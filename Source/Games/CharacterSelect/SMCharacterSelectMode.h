// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/SMGameMode.h"

#include "SMCharacterSelectMode.generated.h"

class ASMCharacterSelectState;

/**
 *
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectMode : public ASMGameMode
{
	GENERATED_BODY()

public:
	ASMCharacterSelectMode();

	virtual void InitGameState() override;

	virtual void StartPlay() override;

	UFUNCTION(BlueprintPure)
	ASMCharacterSelectState* GetCharacterSelectState() const { return CharacterSelectState.Get(); }

	UFUNCTION(BlueprintPure)
	bool IsAllPlayerReadyToSelect() const;

	UFUNCTION(BlueprintCallable)
	void ImmediateStart();

protected:
	UFUNCTION()
	void WaitingTimerTick();

	UFUNCTION()
	void OnCharacterSelectCountdownFinished();

	UFUNCTION()
	void StartGame();

private:
	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;

	UPROPERTY()
	FTimerHandle WaitingTimerHandle;

	UPROPERTY()
	int32 RemainingWaitingTime = 0;
};
