// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreMusicManagerComponent.h"

#include "FMODBlueprintStatics.h"
#include "Games/SMGameState.h"
#include "Games/Round/SMRoundComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

const FName USMScoreMusicManagerComponent::ScoreMusicParameterName = TEXT("Winner");

USMScoreMusicManagerComponent::USMScoreMusicManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void USMScoreMusicManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ScoreMusicPlayingTeam);
}

void USMScoreMusicManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void USMScoreMusicManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld() && GetNetMode() == NM_Client)
	{
		if (ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>())
		{
			GameState->StartListeningGameReadyToStart(FSMOnGameReadyToStart::FDelegate::CreateUObject(this, &ThisClass::PlayScoreMusic));
		}
		if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
		{
			RoundComponent->OnRoundStateUpdatedEvent.AddUObject(this, &ThisClass::OnRoundStateUpdated);
		}
	}
}

void USMScoreMusicManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopScoreMusic();

	Super::EndPlay(EndPlayReason);
}

void USMScoreMusicManagerComponent::PlayScoreMusic()
{
	NET_LOG(GetOwner(), Warning, TEXT("음악 재생 시작"));
	ScoreMusicEventInstance = UFMODBlueprintStatics::PlayEvent2D(this, ScoreMusic, true);
	UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterNone);
}

void USMScoreMusicManagerComponent::StopScoreMusic()
{
	if (ScoreMusicEventInstance.Instance)
	{
		NET_LOG(GetOwner(), Warning, TEXT("음악 종료 %p"), ScoreMusicEventInstance.Instance);
		UFMODBlueprintStatics::EventInstanceStop(ScoreMusicEventInstance, true);
	}
}

void USMScoreMusicManagerComponent::SetScoreMusicPlayingTeam(ESMTeam NewTeam)
{
	ScoreMusicPlayingTeam = NewTeam;
}

void USMScoreMusicManagerComponent::OnRep_ScoreMusicPlayingTeam()
{
	if (ScoreMusicEventInstance.Instance->isValid())
	{
		NET_LOG(GetOwner(), Log, TEXT("%s로 ScoreMusic 변경"), *UEnum::GetValueAsString(ScoreMusicPlayingTeam))
		switch (ScoreMusicPlayingTeam)
		{
			case ESMTeam::None:
			{
				UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterNone);
				break;
			}
			case ESMTeam::EDM:
			{
				UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterEDM);
				break;
			}
			case ESMTeam::FutureBass:
			{
				UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterFB);
				break;
			}
			case ESMTeam::Max:
			{
				break;
			}
		}
	}
}

void USMScoreMusicManagerComponent::OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState)
{
}
