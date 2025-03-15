// Copyright Studio Surround. All Rights Reserved.


#include "SMRoundComponent.h"

#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/SMGlobalAbilitySystem.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/LevelSequenceActor/SMLevelSequenceActor.h"
#include "Components/Character/SMCharacterLandingComponent.h"
#include "Components/Character/SMCharacterLevelTransitionVisualComponent.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Data/SMTeam.h"
#include "Games/SMGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Match/SMMatchDefinition.h"
#include "Match/SMMatchSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "UI/Transition/SMTransitionManager.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMRoundComponent)

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixRound, Log, All);

USMRoundComponent::USMRoundComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	bWantsInitializeComponent = true;
	RoundWinner = ESMTeam::None;
	RoundState = ESMRoundState::None;
	PreviousRoundState = ESMRoundState::None;

	LockTagsOnPreRound.AddTag(SMTags::Character::State::Common::Uncontrollable);
	LockTagsOnPreRound.AddTag(SMTags::Character::State::Common::Blocking::Attack);
	LockTagsOnPreRound.AddTag(SMTags::Character::State::Common::Blocking::Skill);
	LockTagsOnPreRound.AddTag(SMTags::Character::State::Common::Blocking::NoiseBreak);
	LockTagsOnPreRound.AddTag(SMTags::Character::State::Common::Blocking::Sampling);
}

USMRoundComponent* USMRoundComponent::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->GetComponentByClass<USMRoundComponent>();
		}
		UE_LOG(LogStereoMixRound, Error, TEXT("Failed to get game state."));
	}
	return nullptr;
}

void USMRoundComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (GetWorld())
	{
		if (HasAuthority())
		{
			InitializeRound();
		}
	}
}

void USMRoundComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RoundState, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RoundTime, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RemainRoundTime, SharedParams);

	SharedParams.Condition = COND_InitialOnly;
	// RoundCount, Round는 게임 시작 시에만 설정되므로 초기화 시에만 전송합니다.
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RoundCount, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Round, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RoundScore_0, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RoundScore_1, SharedParams);
}

bool USMRoundComponent::ShouldShowLoadingScreen(FString& OutReason)
{
	if (GetRoundState() <= ESMRoundState::Waiting)
	{
		OutReason = TEXT("Round is not started yet.");
		return true;
	}
	return false;
}

int32 USMRoundComponent::GetRoundScore(ESMTeam Team) const
{
	switch (Team)
	{
		case ESMTeam::EDM:
			return RoundScore_0;
		case ESMTeam::FutureBass:
			return RoundScore_1;
		case ESMTeam::None:
			return 0;
		default:
			ensureAlwaysMsgf(true, TEXT("Invalid team: %d"), static_cast<int32>(Team));
			return 0;
	}
}

void USMRoundComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::Waiting);
		LockAbilities();

		if (GetWorld()->bMatchStarted)
		{
			HandleGameReadyToStart();
		}
		else
		{
			GetWorld()->OnWorldMatchStarting.AddUObject(this, &ThisClass::HandleGameReadyToStart);
		}

		// if (ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>())
		// {
		// 	// if (ASMGameMode* GameMode = Cast<ASMGameMode>(GameState->AuthorityGameMode))
		// 	// {
		// 	// 	// 라운드 컴포넌트가 있는 게임모드는 WaitingToStart에서 대기하다가 수동으로 InProgress로 전환해야 합니다.
		// 	// 	GameMode->bDelayedStart = true;
		// 	// }
		// 	if (GameState->IsReadyToStart())
		// 	{
		// 		HandleGameReadyToStart();
		// 	}
		// 	else
		// 	{
		// 		GameState->OnGameReadyToStartEvent.AddUObject(this, &ThisClass::HandleGameReadyToStart);
		// 	}
		// }
	}

	if (GetNetMode() == NM_Client)
	{
		if (RoundState != ESMRoundState::Waiting)
		{
			// 중간에 난입하거나 알 수 없는 이유로 게임이 시작된 후에 클라이언트가 접속했을 때는 바로 트랜지션을 실행합니다.
			if (USMTransitionManager* TransitionManager = USMTransitionManager::Get(this))
			{
				TransitionManager->DoTransition(nullptr, ESMTransitionType::Out);
			}
		}

		if (auto TileManager = USMTileManagerComponent::Get(this))
		{
			TileManager->OnVictoryTeamAnnounced.AddDynamic(this, &ThisClass::HandleVictoryTeamAnnounced);
		}
	}
}

void USMRoundComponent::ManualStartRound()
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		if (RoundState == ESMRoundState::PreRound)
		{
			StartRoundTimer(PreRoundTime);
		}
		else
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("The round is not PreRound"));
		}
	}
#endif
}

void USMRoundComponent::ManualFinishRound()
{
	if (Round == RoundCount)
	{
		BeginResult();
	}
	else
	{
		// TODO: 점수 체크 필요
		TravelToNextMap(Round + 1);
	}
}

void USMRoundComponent::ManualEndRound()
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		if (RoundState == ESMRoundState::InRound)
		{
			StopRoundTimer();
			SetRemainRoundTime(0.0f);
			BeginPostRound();
		}
		else
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("The round is not in progress."));
		}
	}
#endif
}

void USMRoundComponent::InitializeRound()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USMMatchSubsystem* Match = GameInstance->GetSubsystem<USMMatchSubsystem>();
	if (!Match)
	{
		return;
	}

	const USMMatchDefinition* MatchDefinition = Match->GetMatchDefinition();
	if (!MatchDefinition)
	{
		return;
	}
	RoundCount = MatchDefinition->GetRoundCount();

	ASMGameMode* GameMode = World->GetAuthGameMode<ASMGameMode>();
	if (!GameMode)
	{
		return;
	}

	FString Options = GameMode->OptionsString;

	if (FParse::Value(*Options, TEXT("Score0="), RoundScore_0))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Received Score0 from Options: %d"), RoundScore_0);
	}
	if (FParse::Value(*Options, TEXT("Score1="), RoundScore_1))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Received Score1 from Options: %d"), RoundScore_1);
	}

	if (FParse::Value(*Options, TEXT("Round="), Round))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Received Current Round from Options: %d"), Round);
	}
	else
	{
		Round = 1;
	}

	if (FParse::Value(*Options, TEXT("PreRoundTime="), PreRoundTime))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Received PreRoundTime from Options: %f"), PreRoundTime);
	}
	if (FParse::Value(*Options, TEXT("RoundTime="), GameRoundTime))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Received RoundTime from Options: %f"), GameRoundTime);
	}
	if (FParse::Value(*Options, TEXT("ResultTime="), ResultTime))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Received ResultTime from Options: %f"), ResultTime);
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RoundCount, this);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Round, this);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RoundScore_0, this);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RoundScore_1, this);

	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Log,
		TEXT("Round Initialized: RoundCount=%d, Round=%d, PreRoundTime=%f, RoundTime=%f, ResultTime=%f, RoundScores=[%d, %d]"),
		RoundCount,
		Round,
		PreRoundTime,
		GameRoundTime,
		ResultTime,
		RoundScore_0,
		RoundScore_1);

	bPendingNextMap = false;
}

void USMRoundComponent::TravelToNextMap(int32 NextRound)
{
#if WITH_SERVER_CODE
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	USMMatchSubsystem* Match = GetWorld()->GetGameInstance()->GetSubsystem<USMMatchSubsystem>();
	if (!Match)
	{
		return;
	}

	const USMMatchDefinition* MatchDefinition = Match->GetMatchDefinition();
	if (!MatchDefinition)
	{
		return;
	}

	FString NextUrl;
	if (NextRound > 0)
	{
		NextUrl = MatchDefinition->GetRoundUrl(NextRound);
		NextUrl += FString::Printf(TEXT("?Score0=%d?Score1=%d"), RoundScore_0, RoundScore_1);
	}
	else
	{
		if (Match->GetServerType() == ESMMatchServerType::CustomMatch)
		{
			NextUrl = MatchDefinition->GetLobbyUrl();
		}
		else
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Log, TEXT("All rounds are finished and the game is over. Just wait for all players to leave."));
			return;
		}
	}

	if (!NextUrl.IsEmpty())
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("NEXT URL: %s"), *NextUrl);
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Log, TEXT("Travel to the next map."));
		World->ServerTravel(NextUrl, true);
		bPendingNextMap = true;
	}

#endif
}

bool USMRoundComponent::ShouldPlayNextRound() const
{
	if (RoundWinner == ESMTeam::None)
	{
		return Round < RoundCount;
	}
	return Round < RoundCount && GetRoundScore(RoundWinner) < RoundCount / 2 + 1;
}

void USMRoundComponent::StartRoundTimer(float InTime)
{
	if (HasAuthority())
	{
		if (RoundTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(RoundTimerHandle);
			RoundTimerHandle.Invalidate();
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("StartRoundTimer() called but the round timer is already running. The timer is reset."));
		}

		SetActiveRoundTime(InTime);
		SetRemainRoundTime(InTime);
		GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ThisClass::HandleRoundTimerTick, RoundTimerInterval, true, RoundTimerInterval);
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Round Timer Started: %fs"), InTime);
	}
}

void USMRoundComponent::StopRoundTimer()
{
	if (HasAuthority())
	{
		if (RoundTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(RoundTimerHandle);
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Round Timer Stopped."));
		}
		else
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("StopRoundTimer() called but the round timer is not running."));
		}
	}
}

void USMRoundComponent::SetRoundState(ESMRoundState NewRoundState)
{
	if (HasAuthority())
	{
		// 현재 라운드 상태보다 높은 상태로만 변경할 수 있습니다.
		if (RoundState < NewRoundState)
		{
			const ESMRoundState PrevRoundState = RoundState;
			RoundState = NewRoundState;
			PreviousRoundState = PrevRoundState;

			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Round State Changed: %s >> %s"), *UEnum::GetValueAsString(PreviousRoundState), *UEnum::GetValueAsString(RoundState));

			BroadcastRoundStateUpdated(PrevRoundState, RoundState);
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RoundState, this);
		}
		else
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("The round state cannot be changed to a lower state."));
		}
	}
}

void USMRoundComponent::SetActiveRoundTime(float InTime)
{
	if (HasAuthority())
	{
		RoundTime = FMath::Max(InTime, 0.0f);
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RoundTime, this);
	}
}

void USMRoundComponent::SetRemainRoundTime(float InRemainTime)
{
	if (HasAuthority())
	{
		RemainRoundTime = FMath::Max(InRemainTime, 0.0f);

		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Remain Round Time: %fs"), RemainRoundTime);

		BroadcastRoundTimeUpdated(RemainRoundTime);
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RemainRoundTime, this);
	}
}

// void USMRoundComponent::StartCharacterLandingOnLocal()
// {
// 	UWorld* World = GetWorld();
// 	if (!World)
// 	{
// 		return;
// 	}
//
// 	ASMPlayerController* LocalController = Cast<ASMPlayerController>(World->GetFirstPlayerController());
// 	if (!LocalController)
// 	{
// 		return;
// 	}
// 	ASMPlayerCharacterBase* LocalPlayerCharacter = Cast<ASMPlayerCharacterBase>(LocalController->GetPawn());
// 	if (!LocalPlayerCharacter)
// 	{
// 		return;
// 	}
//
// 	const ESMTeam LocalPlayerTeam = LocalPlayerCharacter->GetTeam();
//
// 	ASMWorldSettings* WorldSettings = Cast<ASMWorldSettings>(World->GetWorldSettings());
// 	if (!WorldSettings)
// 	{
// 		return;
// 	}
//
// 	TSoftObjectPtr<ULevelSequence> CountdownLevelSequencePtr = WorldSettings->RoundCountdownLevelSequence[LocalPlayerTeam];
// 	TObjectPtr<ULevelSequence> CountdownLevelSequence = CountdownLevelSequencePtr.LoadSynchronous();
// 	if (!CountdownLevelSequence)
// 	{
// 		return;
// 	}
//
// 	FMovieSceneSequencePlaybackSettings PlaybackSettings;
// 	PlaybackSettings.bAutoPlay = true;
// 	ALevelSequenceActor* LevelSequenceActor;
// 	ULevelSequencePlayer::CreateLevelSequencePlayer(this, CountdownLevelSequence, PlaybackSettings, LevelSequenceActor);
//
// 	TArray<ASMPlayerCharacterBase*> AllyPlayerCharacters;
// 	TArray<ASMPlayerCharacterBase*> EnemyPlayerCharacters;
// 	for (ASMPlayerCharacterBase* Character : TActorRange<ASMPlayerCharacterBase>(GetWorld()))
// 	{
// 		if (USMTeamBlueprintLibrary::IsSameLocalTeam(Character))
// 		{
// 			AllyPlayerCharacters.Add(Character);
// 		}
// 		else
// 		{
// 			EnemyPlayerCharacters.Add(Character);
// 		}
// 	}
//
// 	auto CharacterTypeSort = [](const ASMPlayerCharacterBase& LHS, const ASMPlayerCharacterBase& RHS) { return LHS.GetCharacterType() < RHS.GetCharacterType(); };
// 	AllyPlayerCharacters.Sort(CharacterTypeSort);
// 	EnemyPlayerCharacters.Sort(CharacterTypeSort);
//
// 	auto LandCharacter = [this, World](const TArray<ASMPlayerCharacterBase*>& Characters) {
// 		for (int32 i = 0; i < Characters.Num(); ++i)
// 		{
// 			const ASMPlayerCharacterBase* Character = Characters[i];
// 			USMCharacterLandingComponent* LandingComponent = Character ? Character->GetComponentByClass<USMCharacterLandingComponent>() : nullptr;
// 			auto LandLambda = [LandingComponent] { LandingComponent->PlayLanding(); };
//
// 			if (i == 0)
// 			{
// 				LandLambda();
// 			}
// 			else
// 			{
// 				FTimerHandle TimerHandle;
// 				World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(LandingComponent, LandLambda), 0.15f * i, false);
// 			}
// 		}
// 	};
//
// 	auto AllyLand = [LandCharacter, AllyPlayerCharacters] {
// 		LandCharacter(AllyPlayerCharacters);
// 	};
//
// 	auto EnemyLand = [LandCharacter, EnemyPlayerCharacters] {
// 		LandCharacter(EnemyPlayerCharacters);
// 	};
//
// 	const float SequencePlayTime = PreRoundTime - 1.0f;
// 	const float Interval = SequencePlayTime / 3.0f;
//
// 	FTimerHandle AllyLandTimerHandle;
// 	World->GetTimerManager().SetTimer(AllyLandTimerHandle, FTimerDelegate::CreateWeakLambda(this, AllyLand), Interval * 2, false);
//
// 	FTimerHandle EnemyLandTimerHandle;
// 	World->GetTimerManager().SetTimer(EnemyLandTimerHandle, FTimerDelegate::CreateWeakLambda(this, EnemyLand), Interval, false);
// }

void USMRoundComponent::HandleGameReadyToStart()
{
	static float DelayAfterReadyToStart = 3.0f;

	if (HasAuthority())
	{
		// 1라운드에만 레벨 시퀀스 재생
		FTimerHandle TimerHandle;
		if (GetActiveRound() == 1)
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::BeginLevelSequence, DelayAfterReadyToStart, false);
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::BeginPreRound, DelayAfterReadyToStart, false);
		}
	}
}

void USMRoundComponent::HandleRoundTimerTick()
{
	if (HasAuthority())
	{
		SetRemainRoundTime(RemainRoundTime - RoundTimerInterval);
		if (RemainRoundTime <= 0)
		{
			StopRoundTimer();
			HandleRoundTimeEnd();
		}
	}
}

void USMRoundComponent::HandleRoundTimeEnd()
{
	if (HasAuthority())
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Round Time Ended."));
		switch (GetRoundState())
		{
			case ESMRoundState::PreRound:
				BeginInRound();
				break;

			case ESMRoundState::InRound:
				BeginPostRound();
				break;

			case ESMRoundState::Result:
				TravelToNextMap();
				break;

			default:
				NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("The round time has ended, but the round state is not in progress."));
				break;
		}
	}
}

void USMRoundComponent::BeginLevelSequence()
{
	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Begin Level Sequence."));
	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::LevelSequence);
		if (ASMLevelSequenceActor* LevelSequenceActor = Cast<ASMLevelSequenceActor>(UGameplayStatics::GetActorOfClass(this, ASMLevelSequenceActor::StaticClass())))
		{
			LevelSequenceActor->OnLevelSequenceFinishedEvent.AddUObject(this, &ThisClass::BeginPreRound);
			LevelSequenceActor->PlaySequence();
		}
		else
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Warning, TEXT("Level Sequence Actor is not found."));
			BeginPreRound();
		}
	}
}

void USMRoundComponent::BeginPreRound()
{
	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Begin Pre Round."));
	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::PreRound);

		if (bAutoStartRound)
		{
			StartRoundTimer(PreRoundTime);
		}
	}
}

void USMRoundComponent::BeginInRound()
{
	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Begin In Round."));
	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::InRound);
		UnlockAbilities();
		StartRoundTimer(GameRoundTime);
	}
}

void USMRoundComponent::BeginPostRound()
{
	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Begin Post Round."));
	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::PostRound);

		USMGlobalAbilitySystem& GlobalAbilitySystem = USMGlobalAbilitySystem::Get(this);
		GlobalAbilitySystem.ApplyTagToAll(SMTags::Character::State::Common::Blocking::Sampling);

		if (USMTileManagerComponent* TileManager = USMTileManagerComponent::Get(this))
		{
			TileManager->ShowGameResult();
			RoundWinner = TileManager->CalculateVictoryTeam();

			if (RoundWinner == ESMTeam::None)
			{
				++RoundScore_0;
				++RoundScore_1;
				NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Log, TEXT("Draw. No winner."));
			}
			else
			{
				int32 WinnerRoundScore = 0;
				if (RoundWinner == ESMTeam::EDM)
				{
					// EDM 승
					WinnerRoundScore = ++RoundScore_0;
				}
				else if (RoundWinner == ESMTeam::FutureBass)
				{
					// FutureBass 승
					WinnerRoundScore = ++RoundScore_1;
				}
				NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Log, TEXT("Round Winner: %s, Round Score: %d"), *UEnum::GetValueAsString(RoundWinner), WinnerRoundScore);
			}
		}

		if (bAutoEndRound)
		{
			bool bShouldPlayNextRound = ShouldPlayNextRound();

			if (bShouldPlayNextRound)
			{
				if (AGameStateBase* GameState = GetGameState<AGameStateBase>())
				{
					for (APlayerState* PlayerState : GameState->PlayerArray)
					{
						const APawn* ControlledPawn = PlayerState ? PlayerState->GetPawn() : nullptr;
						if (USMCharacterLevelTransitionVisualComponent* LevelTransitionVisualComponent = ControlledPawn ? ControlledPawn->GetComponentByClass<USMCharacterLevelTransitionVisualComponent>() : nullptr)
						{
							LevelTransitionVisualComponent->ActivateOnServer();
						}
					}
				}

				TravelToNextMapWhenAllPlayersReady();
			}
			else
			{
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, [this] { BeginResult(); }, 2.0f, false);
			}

			if (GetNetMode() == NM_Client && bShouldPlayNextRound)
			{
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, [this] {
					if (USMTransitionManager* TransitionManager = USMTransitionManager::Get(this))
					{
						TransitionManager->DoTransition(ESMTransitionType::In);
					}
				}, 2.0f, false);
			}
		}
	}
}

void USMRoundComponent::BeginResult()
{
	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("Begin Result."));
	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::Result);
		StartRoundTimer(ResultTime);

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerActor = Iterator->Get();
			if (PlayerActor->GetPawn())
			{
				PlayerActor->GetPawn()->Destroy();
			}
		}

		if (USMScoreManagerComponent* ScoreManager = USMScoreManagerComponent::Get(this))
		{
			ScoreManager->ShowResultToAllPlayers(RoundWinner);
		}
	}
}

void USMRoundComponent::TravelToNextMapWhenAllPlayersReady()
{
	AGameStateBase* GameState = GetGameState<AGameStateBase>();
	if (!GameState)
	{
		return;
	}

	bool bIsAllPlayersReady = true;
	for (const TObjectPtr<APlayerState>& PlayerState : GameState->PlayerArray)
	{
		const APawn* ControlledPawn = PlayerState->GetPawn();
		const USMCharacterLevelTransitionVisualComponent* LevelTransitionComponent = ControlledPawn ? ControlledPawn->GetComponentByClass<USMCharacterLevelTransitionVisualComponent>() : nullptr;
		if (LevelTransitionComponent && !LevelTransitionComponent->IsTransitionEnded())
		{
			bIsAllPlayersReady = false;
			break;
		}
	}

	if (bIsAllPlayersReady)
	{
		NET_LOG(GetOwner(), Warning, TEXT("모든 플레이어 준비 완료"));
		TravelToNextMap(Round + 1);
	}
	else
	{
		NET_LOG(GetOwner(), Warning, TEXT("잠시 후 플레이어가 준비되었는지 다시 체크"));
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::TravelToNextMapWhenAllPlayersReady), 0.5f, false);
	}
}

void USMRoundComponent::BroadcastRoundStateUpdated(ESMRoundState InPreviousRoundState, ESMRoundState InNewRoundState) const
{
	OnRoundStateUpdatedEvent.Broadcast(PreviousRoundState, RoundState);
	K2_OnRoundStateUpdatedEvent.Broadcast(PreviousRoundState, RoundState);
}

void USMRoundComponent::BroadcastRoundTimeUpdated(float InUpdatedTime) const
{
	OnRemainRoundTimeUpdatedEvent.Broadcast(InUpdatedTime);
	K2_OnRemainRoundTimeUpdatedEvent.Broadcast(InUpdatedTime);
}

void USMRoundComponent::LockAbilities() const
{
	if (HasAuthority())
	{
		USMGlobalAbilitySystem& GlobalAbilitySystem = USMGlobalAbilitySystem::Get(this);
		for (const FGameplayTag& LockTagOnPreRound : LockTagsOnPreRound)
		{
			GlobalAbilitySystem.ApplyTagToAll(LockTagOnPreRound);
		}
	}
}

void USMRoundComponent::UnlockAbilities() const
{
	if (HasAuthority())
	{
		USMGlobalAbilitySystem& GlobalAbilitySystem = USMGlobalAbilitySystem::Get(this);
		for (const FGameplayTag& LockTagOnPreRound : LockTagsOnPreRound)
		{
			GlobalAbilitySystem.RemoveTagFromAll(LockTagOnPreRound);
		}
	}
}

void USMRoundComponent::HandleVictoryTeamAnnounced(ESMTeam VictoryTeam)
{
	RoundWinner = VictoryTeam;
}

void USMRoundComponent::OnRep_RoundState(ESMRoundState InPreviousRoundState)
{
	// 레플리케이션은 바로바로 동기화가 이뤄지진 않아서 RPC를 사용해야 제대로 연출이 가능하긴 할거임

	NET_LOG_CATEGORY(GetOwner(), LogStereoMixRound, Verbose, TEXT("OnRep_RoundState: %s >> %s"), *UEnum::GetValueAsString(InPreviousRoundState), *UEnum::GetValueAsString(RoundState));
	PreviousRoundState = InPreviousRoundState;
	BroadcastRoundStateUpdated(PreviousRoundState, RoundState);

	if (RoundState == ESMRoundState::Waiting)
	{
	}
	else if (RoundState == ESMRoundState::LevelSequence)
	{
		BeginLevelSequence();
	}
	else if (RoundState == ESMRoundState::PreRound)
	{
		BeginPreRound();
	}
	else if (RoundState == ESMRoundState::InRound)
	{
		BeginInRound();
	}
	else if (RoundState == ESMRoundState::PostRound)
	{
		BeginPostRound();
	}
}

void USMRoundComponent::OnRep_RoundTime()
{
}

void USMRoundComponent::OnRep_RemainRoundTime()
{
	BroadcastRoundTimeUpdated(RemainRoundTime);
}
