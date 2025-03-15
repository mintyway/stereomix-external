// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/SMGameState.h"

#include "SMCharacterSelectState.generated.h"

enum class ESMCharacterType : uint8;
class ASMPlayerState;
class USMCountdownTimerComponent;

UENUM(BlueprintType)
enum class ECharacterSelectionStateType : uint8
{
	Wait,
	Select,
	End
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterSelectionStateChangedEvent, ECharacterSelectionStateType, NewCharacterSelectionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerCharacterFocusChanged, ASMPlayerState*, Player, ESMCharacterType, NewCharacter);


UCLASS()
class STEREOMIX_API ASMCharacterSelectState : public ASMGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Countdown)
	int32 CharacterSelectCountdownTime;

	UPROPERTY(BlueprintAssignable)
	FPlayerCharacterFocusChanged OnPlayerCharacterFocusChanged;

	UPROPERTY(BlueprintAssignable)
	FCharacterSelectionStateChangedEvent OnCurrentStateChanged;

	ASMCharacterSelectState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	virtual bool ShouldShowLoadingScreen(FString& OutReason) override;

	UFUNCTION(BlueprintCallable)
	USMCountdownTimerComponent* GetCountdownTimer() const { return CountdownTimer.Get(); }

	UFUNCTION(BlueprintPure)
	ECharacterSelectionStateType GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintAuthorityOnly)
	void SetCurrentState(ECharacterSelectionStateType NewState);

	void NotifyPlayerCharacterFocusChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter);

private:
	UPROPERTY()
	TObjectPtr<USMCountdownTimerComponent> CountdownTimer;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_CurrentState)
	ECharacterSelectionStateType CurrentState;

	UFUNCTION()
	void OnRep_CurrentState() const;
};
