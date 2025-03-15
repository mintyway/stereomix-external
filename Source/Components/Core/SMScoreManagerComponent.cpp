// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreManagerComponent.h"

#include "GameFramework/GameStateBase.h"
#include "SMTileManagerComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Games/Round/SMRoundComponent.h"
#include "Player/SMPlayerState.h"
#include "UI/ScoreBoard/SMScoreboard.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixScore, Log, All);

USMScoreManagerComponent::USMScoreManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

USMScoreManagerComponent* USMScoreManagerComponent::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->GetComponentByClass<USMScoreManagerComponent>();
		}
		UE_LOG(LogStereoMixScore, Error, TEXT("Failed to get game state."));
	}
	return nullptr;
}

void USMScoreManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	AGameStateBase* GameState = World ? World->GetGameState() : nullptr;

	if (HasAuthority())
	{
		if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
		{
			RoundComponent->OnRoundStateUpdatedEvent.AddUObject(this, &ThisClass::OnRoundStateUpdated);
		}
	}

	ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	if (APlayerController* PlayerController = LocalPlayer ? LocalPlayer->GetPlayerController(World) : nullptr)
	{
		ScoreboardWidget = CreateWidget<USMScoreboard>(PlayerController, ScoreboardWidgetClass);
	}

	if (USMTileManagerComponent* TileManager = GameState ? GameState->GetComponentByClass<USMTileManagerComponent>() : nullptr)
	{
		TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::AddTotalCapturedTiles);
		TileManager->OnVictoryTeamAnnounced.AddDynamic(this, &ThisClass::OnVictoryTeamAnnouncedCallback);
	}
}

void USMScoreManagerComponent::AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount)
{
	if (!CapturedInstigator)
	{
		return;
	}

	const APawn* TargetPawn = Cast<APawn>(CapturedInstigator);
	const AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	if (ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr)
	{
		PlayerScoreData.FindOrAdd(MakeWeakObjectPtr(TargetPlayerState), GetDefaultPlayerScoreData(MakeWeakObjectPtr(CapturedInstigator))).TotalCapturedTiles += CapturedTilesCount;
		TargetPlayerState->SetTempPlayerScore(GetPlayerScoreData(TargetPlayerState));
	}
}

void USMScoreManagerComponent::AddTotalDamageDealt(const AActor* TargetPlayer, float DamageDealt)
{
	if (!TargetPlayer)
	{
		return;
	}

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer);
	const AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	if (ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr)
	{
		PlayerScoreData.FindOrAdd(MakeWeakObjectPtr(TargetPlayerState), GetDefaultPlayerScoreData(MakeWeakObjectPtr(TargetPlayer))).TotalDamageDealt += DamageDealt;
		TargetPlayerState->SetTempPlayerScore(GetPlayerScoreData(TargetPlayerState));
	}
}

void USMScoreManagerComponent::AddTotalDamageReceived(const AActor* TargetPlayer, float DamageReceived)
{
	if (!TargetPlayer)
	{
		return;
	}

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer);
	const AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	if (ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr)
	{
		PlayerScoreData.FindOrAdd(MakeWeakObjectPtr(TargetPlayerState), GetDefaultPlayerScoreData(MakeWeakObjectPtr(TargetPlayer))).TotalDamageReceived += DamageReceived;
		TargetPlayerState->SetTempPlayerScore(GetPlayerScoreData(TargetPlayerState));
	}
}

void USMScoreManagerComponent::AddTotalDeathCount(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer);
	const AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	if (ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr)
	{
		PlayerScoreData.FindOrAdd(MakeWeakObjectPtr(TargetPlayerState), GetDefaultPlayerScoreData(MakeWeakObjectPtr(TargetPlayer))).TotalDeathCount += 1;
		TargetPlayerState->SetTempPlayerScore(GetPlayerScoreData(TargetPlayerState));
	}
}

void USMScoreManagerComponent::AddTotalKillCount(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer);
	const AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	if (ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr)
	{
		PlayerScoreData.FindOrAdd(MakeWeakObjectPtr(TargetPlayerState), GetDefaultPlayerScoreData(MakeWeakObjectPtr(TargetPlayer))).TotalKillCount += 1;
		TargetPlayerState->SetTempPlayerScore(GetPlayerScoreData(TargetPlayerState));
	}
}

void USMScoreManagerComponent::AddTotalNoiseBreakUsage(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer);
	const AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	if (ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr)
	{
		PlayerScoreData.FindOrAdd(MakeWeakObjectPtr(TargetPlayerState), GetDefaultPlayerScoreData(MakeWeakObjectPtr(TargetPlayer))).TotalNoiseBreakUsage += 1;
		TargetPlayerState->SetTempPlayerScore(GetPlayerScoreData(TargetPlayerState));
	}
}

FPlayerScoreData USMScoreManagerComponent::GetPlayerScoreData(const ASMPlayerState* TargetPlayerState) const
{
	return PlayerScoreData.FindRef(MakeWeakObjectPtr(TargetPlayerState));
}

TArray<FPlayerScoreData> USMScoreManagerComponent::GetTeamScoreData(ESMTeam Team)
{
	TArray<FPlayerScoreData> SortedPlayerScores;

	for (const auto& Entry : PlayerScoreData)
	{
		if (const FPlayerScoreData& PlayerScore = Entry.Value; PlayerScore.PlayerTeam == Team)
		{
			if (PlayerScore.PlayerState.IsValid())
			{
				FPlayerScoreData PlayerAccumulatedScore = PlayerScore.PlayerState->GetPlayerScore();
				SortedPlayerScores.Add(PlayerScore + PlayerAccumulatedScore);
			}
		}
	}

	SortedPlayerScores.Sort([](const FPlayerScoreData& A, const FPlayerScoreData& B) {
		return A.TotalScore() > B.TotalScore(); // 점수 높은 순으로 정렬
	});

	if (SortedPlayerScores.Num() > 3)
	{
		SortedPlayerScores.SetNum(3);
	}

	return SortedPlayerScores;
}

void USMScoreManagerComponent::LogAllPlayerData()
{
	for (auto& [PlayerActor, ScoreData] : PlayerScoreData)
	{
		LogPlayerData(PlayerActor);
	}
}

void USMScoreManagerComponent::OnVictoryTeamAnnouncedCallback(ESMTeam VictoryTeam)
{
	// MulticastShowPlaylist(VictoryTeam, GetTeamScoreData(ESMTeam::EDM), GetTeamScoreData(ESMTeam::FutureBass));
}

void USMScoreManagerComponent::ShowResultToAllPlayers(ESMTeam VictoryTeam)
{
	TArray<FPlayerScoreData> EDMTeamScoreData = GetTeamScoreData(ESMTeam::EDM);
	TArray<FPlayerScoreData> FBTeamScoreData = GetTeamScoreData(ESMTeam::FutureBass);

	MulticastShowScoreboard(VictoryTeam, EDMTeamScoreData, FBTeamScoreData);
}

void USMScoreManagerComponent::OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState)
{
	if (NewRoundState == ESMRoundState::PostRound)
	{
		LogAllPlayerData();
	}
}

void USMScoreManagerComponent::MulticastShowScoreboard_Implementation(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData)
{
	if (EDMPlayerData.Num() > 0 || FBPlayerData.Num() > 0)
	{
		if (ScoreboardWidget)
		{
			ScoreboardWidget->ShowScoreboard(WinTeam, EDMPlayerData, FBPlayerData);
		}
	}
}

FPlayerScoreData USMScoreManagerComponent::GetDefaultPlayerScoreData(const TWeakObjectPtr<const AActor>& TargetPlayer)
{
	FPlayerScoreData DefaultPlayerScoreData = FPlayerScoreData();

	if (!TargetPlayer.IsValid())
	{
		return DefaultPlayerScoreData;
	}

	const ASMPlayerCharacterBase* TargetPlayerCharacter = Cast<ASMPlayerCharacterBase>(TargetPlayer.Get());

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer.Get());
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr;

	ESMTeam TargetPlayerTeam = TargetPlayerState ? TargetPlayerState->GetTeam() : ESMTeam::None;
	ESMCharacterType TargetPlayerType = TargetPlayerCharacter ? TargetPlayerCharacter->GetCharacterType() : ESMCharacterType::None;
	FString TargetPlayerName = TargetPlayerState ? TargetPlayerState->GetPlayerName() : FString("");

	DefaultPlayerScoreData.PlayerState = MakeWeakObjectPtr(TargetPlayerState);
	DefaultPlayerScoreData.PlayerTeam = TargetPlayerTeam;
	DefaultPlayerScoreData.CharacterType = TargetPlayerType;
	DefaultPlayerScoreData.PlayerName = TargetPlayerName;

	return DefaultPlayerScoreData;
}

void USMScoreManagerComponent::LogPlayerData(const TWeakObjectPtr<const AActor>& TargetPlayer) const
{
	TMap<ESMTeam, FString> PlayerTeamMap = {
		{ ESMTeam::None, "None" },
		{ ESMTeam::FutureBass, "FutureBass" },
		{ ESMTeam::EDM, "EDM" }
	};
	FString* TargetPlayerTeam = PlayerTeamMap.Find(PlayerScoreData[TargetPlayer].PlayerTeam);

	TMap<ESMCharacterType, FString> PlayerTypeMap = {
		{ ESMCharacterType::None, "None" },
		{ ESMCharacterType::ElectricGuitar, "ElectricGuitar" },
		{ ESMCharacterType::Piano, "Piano" },
		{ ESMCharacterType::Bass, "Bass" }
	};
	FString* TargetPlayerType = PlayerTypeMap.Find(PlayerScoreData[TargetPlayer].CharacterType);

	FString TargetPlayerName = PlayerScoreData[TargetPlayer].PlayerName;

	UE_LOG(LogStereoMixScore, Verbose, TEXT("[%s : %s] %s\nTiles: %d, DamageDealt: %f, DamageReceived: %f, Kill: %d, Death: %d, NoiseBreak: %d, TotalScore: %d"),
		**TargetPlayerTeam,
		**TargetPlayerType,
		*TargetPlayerName,
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}
