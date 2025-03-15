// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamComponent.h"

#include "StereoMixLog.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMPlayerState.h"
#include "Utilities/SMLog.h"


USMTeamComponent::USMTeamComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	Team = ESMTeam::None;
}

void USMTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Team, SharedParams);
}

void USMTeamComponent::OnRep_Team(ESMTeam PreviousTeam)
{
	OnTeamChanged.Broadcast(GetOwner(), PreviousTeam, Team);
	K2_OnTeamChangedDynamic.Broadcast(GetOwner(), PreviousTeam, Team);
}

void USMTeamComponent::SetTeam(ESMTeam InTeam)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ESMTeam PrevTeam = Team;
		Team = InTeam;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Team, this);
		OnRep_Team(PrevTeam);
	}
}
