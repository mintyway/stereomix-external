// Copyright Surround, Inc. All Rights Reserved.


#include "SMTileManagerComponent.h"

#include "EngineUtils.h"
#include "SMGameplayTags.h"
#include "SMScoreMusicManagerComponent.h"
#include "StereoMixLog.h"
#include "Actors/Tiles/SMTile.h"
#include "FunctionLibraries/SMScoreFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Games/Round/SMRoundComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


USMTileManagerComponent::USMTileManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);

	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		const ESMTeam Team = static_cast<ESMTeam>(i);
		TileScores.Add(Team, 0);
	}
}

USMTileManagerComponent* USMTileManagerComponent::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->GetComponentByClass<USMTileManagerComponent>();
		}
		UE_LOG(LogStereoMix, Error, TEXT("Failed to get game state."));
	}
	return nullptr;
}

void USMTileManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EDMTileScore);
	DOREPLIFETIME(ThisClass, FBTileScore);
}

void USMTileManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && GetOwnerRole() == ROLE_Authority)
	{
		int32 TotalTileCount = 0;
		for (ASMTile* Tile : TActorRange<ASMTile>(World))
		{
			if (Tile)
			{
				++TotalTileCount;
				Tile->OnChangeTileWithTeamInformation.AddUObject(this, &ThisClass::OnTileChanged);
			}
		}

		NET_LOG(GetOwner(), Warning, TEXT("현재 타일 개수: %d"), TotalTileCount)
	}
}

void USMTileManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* CachedOwner = GetOwner();
	CachedScoreMusicManager = CachedOwner ? CachedOwner->FindComponentByClass<USMScoreMusicManagerComponent>() : nullptr;
}

TArray<ASMTile*> USMTileManagerComponent::CaptureTiles(const TArray<ASMTile*>& TilesToBeCaptured, const AActor* Instigator, const TOptional<ESMTeam>& OverrideTeamOption)
{
	TArray<ASMTile*> CapturedTiles;
	const ESMTeam InstigatorTeam = OverrideTeamOption.Get(USMTeamBlueprintLibrary::GetTeam(Instigator));

	const FColor DebugColor = FColor::MakeRandomColor();
	for (ASMTile* TileToBeCaptured : TilesToBeCaptured)
	{
		const ESMTeam TileTeam = TileToBeCaptured ? TileToBeCaptured->GetTeam() : ESMTeam::None;
		if (!TileToBeCaptured || InstigatorTeam == TileTeam)
		{
			continue;
		}

		TileToBeCaptured->TileTrigger(InstigatorTeam);
		CapturedTiles.Add(TileToBeCaptured);

		if (bShowDebug)
		{
			DrawDebugBox(GetWorld(), TileToBeCaptured->GetTileLocation(), FVector(USMTileFunctionLibrary::DefaultTileSize / 2.0), DebugColor, false, 1.5f);
		}
	}

	if (CapturedTiles.Num() > 0)
	{
		OnTilesCaptured.Broadcast(Instigator, CapturedTiles.Num());
	}

	return CapturedTiles;
}

void USMTileManagerComponent::SetTileScores(ESMTeam Team, int32 Score)
{
	TileScores[Team] = Score;

	EDMTileScore = TileScores[ESMTeam::EDM];
	FBTileScore = TileScores[ESMTeam::FutureBass];

	OnRep_EDMTileScore();
	OnRep_FBTileScore();
}

ESMTeam USMTileManagerComponent::GetHighestScoringTeam() const
{
	if (EDMTileScore == FBTileScore)
	{
		return ESMTeam::None;
	}

	return EDMTileScore > FBTileScore ? ESMTeam::EDM : ESMTeam::FutureBass;
}

void USMTileManagerComponent::ShowGameResult()
{
	MulticastSendGameResult(CalculateVictoryTeam());
}

void USMTileManagerComponent::ResetTileScores()
{
	for (int32 TeamAsInt32 = 1; TeamAsInt32 < static_cast<int32>(ESMTeam::Max); ++TeamAsInt32)
	{
		SetTileScores(static_cast<ESMTeam>(TeamAsInt32), 0);
	}
}

void USMTileManagerComponent::OnTileChanged(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	if (PreviousTeam == NewTeam) // 이미 타일 측에서 팀이 변경되지 않은 경우 처리되지 않도록 예외처리를 진행해두었지만 만약을 위한 방어코드입니다.
	{
		return;
	}

	if (NewTeam == ESMTeam::None) // None 팀으로 점령하는 경우 타일이 다시 중립화 됩니다.
	{
		NeutralizeTile(PreviousTeam);
	}
	else
	{
		if (PreviousTeam == ESMTeam::None) // 중립 타일을 점령하는 경우 스코어에 그대로 더해줍니다.
		{
			AddTileScore(NewTeam);
		}
		else // 이미 다른팀이 점령한 타일을 점령하는 경우 스코어를 스왑해줍니다.
		{
			SwapTileScore(PreviousTeam, NewTeam);
		}
	}

	ChangeScoreMusic();
}

void USMTileManagerComponent::ChangeScoreMusic()
{
	USMRoundComponent* RoundComponent = USMRoundComponent::Get(this)
	if (!CachedScoreMusicManager.IsValid() || !RoundComponent)
	{
		continue;
	}

	if (RoundComponent->GetRoundState() != ESMRoundState::InRound)
	{
		continue;
	}

	if (TileScores[ESMTeam::FutureBass] == TileScores[ESMTeam::EDM])
	{
		CachedScoreMusicManager->SetScoreMusicPlayingTeam(ESMTeam::None);
	}
	else
	{
		const ESMTeam CurrentWinner = TileScores[ESMTeam::EDM] > TileScores[ESMTeam::FutureBass] ? ESMTeam::EDM : ESMTeam::FutureBass;
		if (CachedScoreMusicManager->GetScoreMusicPlayingTeam() != CurrentWinner)
		{
			CachedScoreMusicManager->SetScoreMusicPlayingTeam(CurrentWinner);
		}
	}
}

void USMTileManagerComponent::NeutralizeTile(ESMTeam PreviousTeam)
{
	SetTileScores(PreviousTeam, TileScores[PreviousTeam] - 1);
}

void USMTileManagerComponent::AddTileScore(ESMTeam Team)
{
	SetTileScores(Team, TileScores[Team] + 1);
}

void USMTileManagerComponent::SwapTileScore(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	SetTileScores(PreviousTeam, TileScores[PreviousTeam] - 1);
	SetTileScores(NewTeam, TileScores[NewTeam] + 1);
}

void USMTileManagerComponent::OnRep_EDMTileScore()
{
	TileScores[ESMTeam::EDM] = EDMTileScore;
	OnEDMTileScoreChanged.Broadcast(EDMTileScore);

	const ESMTeam HighestScoringTeam = GetHighestScoringTeam();
	OnHighestScoringTeamChanged.Broadcast(HighestScoringTeam);
	OnNativeHighestScoringTeamChanged.Broadcast(HighestScoringTeam);
}

void USMTileManagerComponent::OnRep_FBTileScore()
{
	TileScores[ESMTeam::FutureBass] = FBTileScore;
	OnFBTileScoreChanged.Broadcast(FBTileScore);

	const ESMTeam HighestScoringTeam = GetHighestScoringTeam();
	OnHighestScoringTeamChanged.Broadcast(HighestScoringTeam);
	OnNativeHighestScoringTeamChanged.Broadcast(HighestScoringTeam);
}

ESMTeam USMTileManagerComponent::CalculateVictoryTeam()
{
	ESMTeam VictoryTeam = ESMTeam::None;
	if (TileScores[ESMTeam::EDM] != TileScores[ESMTeam::FutureBass])
	{
		VictoryTeam = TileScores[ESMTeam::EDM] > TileScores[ESMTeam::FutureBass] ? ESMTeam::EDM : ESMTeam::FutureBass;
	}
	else if (const UWorld* World = GetWorld())
	{
		// 동점인 경우 팀 점수 기준으로 승자를 결정합니다.
		const int32 EDMTeamScore = USMScoreFunctionLibrary::GetTeamScore(World, ESMTeam::EDM);
		const int32 FBTeamScore = USMScoreFunctionLibrary::GetTeamScore(World, ESMTeam::FutureBass);

		VictoryTeam = EDMTeamScore > FBTeamScore ? ESMTeam::EDM : ESMTeam::FutureBass;
	}

	return VictoryTeam;
}

void USMTileManagerComponent::MulticastSendGameResult_Implementation(ESMTeam VictoryTeam)
{
	OnVictoryTeamAnnounced.Broadcast(VictoryTeam);
}
