// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/SMGameState.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SMAsyncAction_WaitUntilReadyToStart.generated.h"


UCLASS()
class STEREOMIX_API USMAsyncAction_WaitUntilReadyToStart : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** 월드 내 모든 플레이어가 준비 될 때까지 대기합니다. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly, WorldContext = "InWorldContextObject"))
	static USMAsyncAction_WaitUntilReadyToStart* WaitUntilReadyToStart(UObject* InWorldContextObject);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FSMOnGameReadyToStartDynamic OnGameReadyToStart;

private:
	void CheckGameStateReadyToStart(AGameStateBase* GameState);
	void HandleGameReadyToStart();
	void HandlePlayerReadyToPlay();

	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	bool bGameStateReadyToPlay = false;
	bool bPlayerReadyToPlay = false;
};
