// Copyright Studio Surround. All Rights Reserved.


#include "SMUserWidget_GameStartCountdown.h"

#include "Components/Core/SMTileManagerComponent.h"
#include "Data/Enum/SMRoundState.h"
#include "Games/Round/SMRoundComponent.h"

void USMUserWidget_GameStartCountdown::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BindGameState();
}

void USMUserWidget_GameStartCountdown::BindGameState()
{
	if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
	{
		RoundComponent->OnRoundStateUpdatedEvent.AddUObject(this, &ThisClass::OnRoundStateUpdated);
		RoundComponent->OnRemainRoundTimeUpdatedEvent.AddUObject(this, &ThisClass::OnRemainRoundTimeUpdated);
	}

	if (USMTileManagerComponent* TileManager = USMTileManagerComponent::Get(this))
	{
		TileManager->OnVictoryTeamAnnounced.AddDynamic(this, &ThisClass::HandleVictoryTeamAnnounced);
	}
}

void USMUserWidget_GameStartCountdown::OnPreRoundTimeExpired()
{
	PlayAnimationForward(StartLetsMix);
	BindGameState();
}

void USMUserWidget_GameStartCountdown::HandleVictoryTeamAnnounced(ESMTeam VictoryTeam)
{
	if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
	{
		if (!RoundComponent->ShouldPlayNextRound())
		{
			PlayAnimationForward(StartMixOver);
		}
	}
}

void USMUserWidget_GameStartCountdown::OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState)
{
	if (NewRoundState == ESMRoundState::PreRound)
	{
		PlayAnimationForward(SequenceCountdown);
	}
	else if (NewRoundState == ESMRoundState::InRound)
	{
		PlayAnimationForward(StartLetsMix);
	}
}

void USMUserWidget_GameStartCountdown::OnRemainRoundTimeUpdated(float RemainTime)
{
	if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
	{
		if (RoundComponent->GetRoundState() == ESMRoundState::InRound)
		{
			// BUG: RemainTime이 갑자기 9를 건너뛰는 경우가 있을 수 있음
			if (RemainTime == 9)
			{
				PlayAnimationForward(StartCountdown);
			}
		}
	}
}
