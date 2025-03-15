// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorInitStateInterface.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "Components/ControllerComponent.h"
#include "UI/CharacterFeed/SMCharacterFeedAvatarDataAsset.h"
#include "SMLobbyPlayerControllerComponent.generated.h"

/**
 * 로비에서 BGM 등의 플레이어 컨트롤러와 관련된 기능이 있는 컴포넌트입니다.
 */
UCLASS(Abstract, DisplayName="StereoMix Lobby Player Controller Component")
class STEREOMIX_API USMLobbyPlayerControllerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	USMLobbyPlayerControllerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ReceivedPlayer() override;

protected:
	void HandleLocalPlayerReadyToPlay(IActorInitStateInterface* ActorToReady);
	void HandleLocalPlayerTeamChanged(AActor* LocalPlayerActor, ESMTeam PreviousTeam, ESMTeam NewTeam);

	void RegisterTeamChangedEvent();
	
	void StartBackgroundMusic();
	void StopBackgroundMusic();
	void UpdateBackgroundMusicParameter(ESMTeam Team);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby|Sound")
	TSoftObjectPtr<UFMODEvent> LobbyBackgroundMusic;

	FFMODEventInstance BackgroundMusicInstance{};
};
