// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/SMRoundState.h"
#include "Player/SMPlayerState.h"
#include "UI/Common/SMUserWidget.h"
#include "SMUserWidget_GameStartCountdown.generated.h"

class ASMGameState;
class USMRoundTimerManagerComponent;

UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_GameStartCountdown : public USMUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	void BindGameState();

	UFUNCTION()
	void OnPreRoundTimeExpired();

	UFUNCTION()
	void HandleVictoryTeamAnnounced(ESMTeam VictoryTeam);
	
	void OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState);
	void OnRemainRoundTimeUpdated(float RemainTime);

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartCountdown;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SequenceCountdown;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartLetsMix;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartMixOver;

	TWeakObjectPtr<ASMGameState> CachedGameState;
	
	TWeakObjectPtr<USMRoundTimerManagerComponent> CachedTimerComponent;
};
