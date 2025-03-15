// Copyright Studio Surround. All Rights Reserved.


#include "SMLobbyPlayerControllerComponent.h"

#include "SMLobbyLog.h"
#include "Audio/SMFMODBlueprintStatics.h"
#include "Components/Common/SMTeamComponent.h"
#include "Player/SMPlayerController.h"
#include "Utilities/SMLog.h"


USMLobbyPlayerControllerComponent::USMLobbyPlayerControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USMLobbyPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		StartBackgroundMusic();
		if (ASMPlayerController* StereoMixPC = GetControllerChecked<ASMPlayerController>())
		{
			if (StereoMixPC->IsReadyToPlay())
			{
				RegisterTeamChangedEvent();
			}
			else
			{
				StereoMixPC->WhenReadyToPlay(FOnActorReadyToPlay::FDelegate::CreateUObject(this, &ThisClass::HandleLocalPlayerReadyToPlay));
			}
		}
	}
}

void USMLobbyPlayerControllerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsLocalController())
	{
		StopBackgroundMusic();
	}
}

void USMLobbyPlayerControllerComponent::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void USMLobbyPlayerControllerComponent::HandleLocalPlayerReadyToPlay(IActorInitStateInterface* ActorToReady)
{
	RegisterTeamChangedEvent();
}

void USMLobbyPlayerControllerComponent::HandleLocalPlayerTeamChanged(AActor* LocalPlayerActor, ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	UpdateBackgroundMusicParameter(NewTeam);
}

void USMLobbyPlayerControllerComponent::RegisterTeamChangedEvent()
{
	if (const ASMPlayerController* StereoMixPC = GetControllerChecked<ASMPlayerController>())
	{
		if (USMTeamComponent* TeamComponent = StereoMixPC->GetTeamComponent())
		{
			TeamComponent->OnTeamChanged.AddUObject(this, &ThisClass::HandleLocalPlayerTeamChanged);
			UpdateBackgroundMusicParameter(TeamComponent->GetTeam());
		}
	}
}

void USMLobbyPlayerControllerComponent::StartBackgroundMusic()
{
	if (USMFMODBlueprintStatics::EventInstanceIsValid(BackgroundMusicInstance))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixLobby, Warning, TEXT("Background music is already playing."));
		return;
	}
	BackgroundMusicInstance = USMFMODBlueprintStatics::PlayEvent2D(this, LobbyBackgroundMusic.LoadSynchronous(), true);
}

void USMLobbyPlayerControllerComponent::StopBackgroundMusic()
{
	if (!USMFMODBlueprintStatics::EventInstanceIsValid(BackgroundMusicInstance))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixLobby, Warning, TEXT("Background music is not playing."));
		return;
	}
	USMFMODBlueprintStatics::EventInstanceStop(BackgroundMusicInstance, true);
}

void USMLobbyPlayerControllerComponent::UpdateBackgroundMusicParameter(ESMTeam Team)
{
	if (!USMFMODBlueprintStatics::EventInstanceIsValid(BackgroundMusicInstance))
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixLobby, Warning, TEXT("Background music is not playing."));
		return;
	}

	float Parameter = 0.0f;
	switch (Team)
	{
		case ESMTeam::None:
			Parameter = 0.0f;
			break;
		case ESMTeam::FutureBass:
			Parameter = 1.0f;
			break;
		case ESMTeam::EDM:
			Parameter = 2.0f;
			break;
		case ESMTeam::Max:
			break;
	}
	UE_LOG(LogStereoMixLobby, Verbose, TEXT("UpdateBackgroundMusicParameter: %f"), Parameter);

	USMFMODBlueprintStatics::EventInstanceSetParameter(BackgroundMusicInstance, TEXT("teamchoice"), Parameter);
}
