// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/Round/SMRoundComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SMAsyncAction_WaitUntilRoundState.generated.h"


UCLASS()
class STEREOMIX_API USMAsyncAction_WaitUntilRoundState : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly, WorldContext = "InWorldContextObject"))
	static USMAsyncAction_WaitUntilRoundState* WaitUntilRoundState(UObject* InWorldContextObject, ESMRoundState RoundState);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FSMOnRoundStateUpdateDynamic OnRoundStateUpdated;

private:
	void HandleGameStateSet(AGameStateBase* GameState);
	void HandleRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState);

	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY(Transient)
	ESMRoundState TargetRoundState;
};
