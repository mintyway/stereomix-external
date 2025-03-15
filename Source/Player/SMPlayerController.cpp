// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerController.h"

#include "GameFramework/GameModeBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "PrimaryGameLayout.h"
#include "SMPlayerState.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/PlayerStart/SMPlayerStartBase.h"
#include "Components/Common/SMTeamComponent.h"
#include "Development/SMCheatManager.h"
#include "Input/SMInputSettings.h"
#include "System/SMWorldLoadingSubsystem.h"
#include "UI/Transition/SMTransitionManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPlayerController)

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixPlayerController, Log, All);

ASMPlayerController::ASMPlayerController()
{
	bShowMouseCursor = true;

#if UE_WITH_CHEAT_MANAGER
	CheatClass = USMCheatManager::StaticClass();
#endif
}

void ASMPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BindPlayerTeamChangedEvent();
	BindPlayerCharacterTypeChangedEvent();
}

UAbilitySystemComponent* ASMPlayerController::GetAbilitySystemComponent() const
{
	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(PlayerState))
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}

bool ASMPlayerController::ShouldShowLoadingScreen(FString& OutReason)
{
	if (!IsReadyToPlay())
	{
		LastLoadingScreenCheckTime = GetWorld()->GetTimeSeconds();
		OutReason = TEXT("Player is not ready to play.");
		return true;
	}

	return false;
}

ESMTeam ASMPlayerController::GetTeam() const
{
	if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
	{
		return StereoMixPS->GetTeam();
	}
	return ESMTeam::None;
}

USMTeamComponent* ASMPlayerController::GetTeamComponent() const
{
	if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
	{
		return StereoMixPS->GetTeamComponent();
	}
	return nullptr;
}

ESMCharacterType ASMPlayerController::GetCharacterType() const
{
	if (const ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
	{
		return StereoMixPS->GetCharacterType();
	}
	return ESMCharacterType::None;
}

void ASMPlayerController::SetCharacterType(ESMCharacterType CharacterType)
{
	if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
	{
		return StereoMixPS->SetCharacterType(CharacterType);
	}
}

FSMOnCharacterChanged* ASMPlayerController::GetOnCharacterChangedDelegate()
{
	if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
	{
		return StereoMixPS->GetOnCharacterChangedDelegate();
	}
	return nullptr;
}

void ASMPlayerController::BeginPlay()
{
	NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("BeginPlay"));
	Super::BeginPlay();

	ResetReadyToPlay();
	WhenReadyToPlay(FOnActorReadyToPlay::FDelegate::CreateUObject(this, &ThisClass::OnControllerReadyToPlay));

	if (IsLocalController())
	{
		if (USMWorldLoadingSubsystem* WorldLoadingSubsystem = GetWorld()->GetSubsystem<USMWorldLoadingSubsystem>())
		{
			if (!WorldLoadingSubsystem->IsLevelLoaded())
			{
				WorldLoadingSubsystem->OnLevelLoadedEvent.AddUObject(this, &ThisClass::OnLevelLoaded);
			}
		}

		bIsUserInterfaceInitialized = false;
		if (bAutoTransitionOnBeginPlay)
		{
			if (USMTransitionManager* TransitionManager = USMTransitionManager::Get(this))
			{
				TransitionManager->DoTransition(ESMTransitionType::Out);
			}
		}
		CheckIsReadyToPlay();
	}
}

void ASMPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PlayerTeamChangedEventHandle.IsValid())
	{
		if (USMTeamComponent* TeamComponent = GetTeamComponent())
		{
			TeamComponent->OnTeamChanged.Remove(PlayerTeamChangedEventHandle);
		}
	}
	if (PlayerCharacterTypeChangedEventHandle.IsValid())
	{
		if (FSMOnCharacterChanged* OnCharacterChanged = GetOnCharacterChangedDelegate())
		{
			OnCharacterChanged->Remove(PlayerCharacterTypeChangedEventHandle);
		}
	}
}

void ASMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();

		TArray<FSMInputMappingContextSetting> Settings;

		if (const USMInputSettings* InputSettings = GetDefault<USMInputSettings>())
		{
			for (const FSMInputMappingContextSetting& Setting : InputSettings->DefaultMappingContexts)
			{
				Settings.Add(Setting);
			}
		}
		for (const FSMInputMappingContextSetting& Setting : InputMappingContextSettings)
		{
			// IMC가 중복되지 않을 때만 추가, 중복되는 경우에는 AddImmediately를 true로 설정
			auto Predicate = [Setting](const FSMInputMappingContextSetting& TargetSetting) { return TargetSetting.InputMapping == Setting.InputMapping; };
			if (FSMInputMappingContextSetting* FoundSetting = Settings.FindByPredicate(Predicate))
			{
				FoundSetting->Priority = Setting.Priority;
				FoundSetting->bAddImmediately = Setting.bAddImmediately;
				FoundSetting->bRegisterWithSettings = Setting.bRegisterWithSettings;
			}
			else
			{
				Settings.Add(Setting);
			}
		}

		UEnhancedInputUserSettings* InputUserSettings = Subsystem->GetUserSettings();
		for (const FSMInputMappingContextSetting& Setting : Settings)
		{
			if (const UInputMappingContext* InputMappingContext = Setting.InputMapping.LoadSynchronous())
			{
				if (Setting.bAddImmediately)
				{
					FModifyContextOptions ModifyContextOptions;
					ModifyContextOptions.bNotifyUserSettings = Setting.bRegisterWithSettings;
					Subsystem->AddMappingContext(InputMappingContext, Setting.Priority, ModifyContextOptions);
					NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("Added IMC: %s, Priority: %d, RegisterSettings: %s"),
						*InputMappingContext->GetFName().ToString(),
						Setting.Priority,
						Setting.bRegisterWithSettings ? TEXT("True") : TEXT("False"));
				}
				else if (Setting.bRegisterWithSettings)
				{
					if (InputUserSettings)
					{
						if (InputUserSettings->RegisterInputMappingContext(InputMappingContext))
						{
							NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("Registered IMC with UEnhancedInputUserSettings: %s"), *InputMappingContext->GetFName().ToString());
						}
					}
				}
			}
		}
	}
	CheckIsReadyToPlay();
}

void ASMPlayerController::PostSeamlessTravel()
{
	NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("PostSeamlessTravel"));
	Super::PostSeamlessTravel();
	bIsReadyToPlay = false;
}

void ASMPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BindPlayerTeamChangedEvent();
	CheckIsReadyToPlay();
}

void ASMPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (APawn* NewPawn = GetPawn())
	{
		// Character에서 초기화를 처리할 수 있다면 Character에서 하는 것이 제일 좋음... 일단은 여기서 초기화 처리함

		if (NewPawn->HasActorBegunPlay())
		{
			CheckIsReadyToPlay();
		}
		else
		{
			bPlayerInitializationDelayInProgress = true;
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [ThisWeakPtr = MakeWeakObjectPtr(this)] {
				if (ThisWeakPtr.IsValid())
				{
					ThisWeakPtr->bPlayerInitializationDelayInProgress = false;
					ThisWeakPtr->CheckIsReadyToPlay();
				}
			}, 0.5f, false);
		}
	}
}

void ASMPlayerController::BindPlayerTeamChangedEvent()
{
	if (USMTeamComponent* TeamComponent = GetTeamComponent())
	{
		PlayerTeamChangedEventHandle = TeamComponent->OnTeamChanged.AddUObject(this, &ThisClass::OnPlayerTeamChanged);
	}
}

void ASMPlayerController::BindPlayerCharacterTypeChangedEvent()
{
	if (FSMOnCharacterChanged* OnCharacterChanged = GetOnCharacterChangedDelegate())
	{
		PlayerCharacterTypeChangedEventHandle = OnCharacterChanged->AddUObject(this, &ThisClass::OnPlayerCharacterTypeChanged);
	}
}

void ASMPlayerController::SetReadyToPlay(bool bReadyToPlay)
{
	bIsReadyToPlay = bReadyToPlay;
}

void ASMPlayerController::CheckIsReadyToPlay()
{
	// 플레이어 컨트롤러의 초기화
	// 1. 클라이언트에서 초기화 체크
	// 2. bShouldInitializePawn이 true일 때, 클라이언트에서 Character의 ReadyToPlay 체크
	// 3. 클라이언트에서 초기화가 끝나면 클라이언트의 ReadyToPlay=true 설정 및 서버로 ReadyToPlay 호출
	// 4. 서버에서도 ReadyToPlay=true 설정

	if (!IsLocalController() || IsReadyToPlay())
	{
		return;
	}

	// BeginPlay 시점에 GameState는 무조건 존재하므로 GameState는 제외
	if (!PlayerState)
	{
		NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: PlayerState is not valid."));
		return;
	}

	// 팀이랑 캐릭터는 로비 레벨에서는 None인 것이 정상이므로 팀, 캐릭터 체크는 폰 액터에서 해야 함

	// if (GetTeam() == ESMTeam::None)
	// {
	// 	NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: Player has no team."));
	// 	return;
	// }
	// if (GetCharacterType() == ESMCharacterType::None)
	// {
	// 	NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: Player has no character type."));
	// 	return;
	// }
	if (bShouldInitializePawn)
	{
		APawn* PlayerPawn = GetPawn();
		if (!PlayerPawn)
		{
			NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: Pawn is not valid."));
			return;
		}
		if (!PlayerPawn->HasActorBegunPlay())
		{
			NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: Pawn has not begun play yet."));
			return;
		}
		if (bPlayerInitializationDelayInProgress)
		{
			NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: Player initialization delay is in progress."));
			return;
		}
	}
	if (!HasActorBegunPlay())
	{
		NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: PlayerController has not begun play yet."));
		return;
	}

	if (USMWorldLoadingSubsystem* WorldLoadingSubsystem = GetWorld()->GetSubsystem<USMWorldLoadingSubsystem>())
	{
		if (!WorldLoadingSubsystem->IsLevelLoaded())
		{
			NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: World is loading."));
			return;
		}
	}

	NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("CheckIsReadyToPlay: PlayerController is ready to play."));

	ServerReadyToPlay();
	ServerReadyToPlay_Implementation();
}

void ASMPlayerController::ServerReadyToPlay_Implementation()
{
	TriggerReadyToPlay(this);
}

void ASMPlayerController::OnControllerReadyToPlay(IActorInitStateInterface* ActorToReady)
{
	if (IsLocalController())
	{
		if (bAutoInitializeUserInterface)
		{
			InitializeUserInterface();
		}
	}
}

void ASMPlayerController::OnLevelLoaded()
{
	NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Verbose, TEXT("HandleLevelLoaded"));
	CheckIsReadyToPlay();
}

void ASMPlayerController::OnPlayerTeamChanged(AActor* ObjectChangingCharacter, ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	if (NewTeam != ESMTeam::None)
	{
		CheckIsReadyToPlay();
	}
}

void ASMPlayerController::OnPlayerCharacterTypeChanged(UObject* ObjectChangingCharacter, ESMCharacterType PreviousCharacterType, ESMCharacterType NewCharacterType)
{
	if (NewCharacterType != ESMCharacterType::None)
	{
		CheckIsReadyToPlay();
	}
}

USMPrimaryLayout* ASMPlayerController::InitializeUserInterface()
{
	if (bIsUserInterfaceInitialized)
	{
		return nullptr;
	}

	if (!PrimaryLayoutClass.IsNull())
	{
		bIsUserInterfaceInitialized = true;

		UClass* LayoutClass = PrimaryLayoutClass.LoadSynchronous();
		if (ensure(LayoutClass))
		{
			if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(this))
			{
				USMPrimaryLayout* NewLayout = RootLayout->PushWidgetToLayerStack<USMPrimaryLayout>(LayerTag, LayoutClass);
				if (ensure(NewLayout))
				{
					PrimaryWidget = NewLayout;
					return NewLayout;
				}
			}
			else
			{
				NET_LOG_CATEGORY(this, LogStereoMixPlayerController, Error, TEXT("Root layout is not found."));
			}
		}
	}
	return nullptr;
}

void ASMPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (PlayerState)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			ASC->InitAbilityActorInfo(PlayerState, InPawn);
		}
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		// 스탠드얼론은 여기서 초기화 체크해야 함
		if (InPawn->HasActorBegunPlay())
		{
			CheckIsReadyToPlay();
		}
		else
		{
			bPlayerInitializationDelayInProgress = true;
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this] {
				bPlayerInitializationDelayInProgress = false;
				CheckIsReadyToPlay();
			}, 2.0f, false);
		}
	}
}

void ASMPlayerController::OnUnPossess()
{
	// ASC의 아바타 액터가 아직 설정되어있다면 해제합니다.
	if (APawn* PawnBeingUnPossess = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnPossess)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}
	Super::OnUnPossess();
}

void ASMPlayerController::AddCheats(bool bForce)
{
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

void ASMPlayerController::RequestImmediateResetPosition_Implementation()
{
#if WITH_SERVER_CODE
	ACharacter* PlayerCharacter = GetCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	const ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(PlayerState);
	if (!SMPlayerState)
	{
		return;
	}

	const ESMTeam Team = SMPlayerState->GetTeam();

	// 플레이어 스타트를 통해 알맞은 스폰 장소를 설정합니다.
	FVector NewLocation;
	FRotator NewRotation;
	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode(); ensureAlways(GameMode))
	{
		FString TeamStarterTag;
		switch (Team)
		{
			case ESMTeam::EDM:
			{
				TeamStarterTag = TEXT("Starter_EDM");
				break;
			}
			case ESMTeam::FutureBass:
			{
				TeamStarterTag = TEXT("Starter_FB");
				break;
			}
			default:
			{
				break;
			}
		}

		if (const AActor* PlayerStarter = GameMode->FindPlayerStart(this, TeamStarterTag); ensureAlways(PlayerStarter))
		{
			NewLocation = PlayerStarter->GetActorLocation();
			NewRotation = PlayerStarter->GetActorRotation();
		}
	}

	PlayerCharacter->TeleportTo(NewLocation, NewRotation);
#endif
}
