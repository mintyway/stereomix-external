// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ControlFlow.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "SMFlowComponent.generated.h"


UCLASS(Abstract, BlueprintType, Blueprintable)
class STEREOMIX_API USMFlowComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool ShouldShowLoadingScreen(FString& OutReason) override;

protected:
	virtual void MakeFlow(FControlFlow& Flow)
	{
	}

	TSharedPtr<FControlFlow> GetFlow() const
	{
		return ActiveFlow;
	}

	void ExecuteFlow() const;

	UFUNCTION()
	virtual void OnNodeComplete();

	UFUNCTION()
	virtual void OnFlowComplete();

	/** LoadingScreen을 보여주어야 하는지 여부를 정합니다. */
	void SetShouldShowLoadingScreen(bool bShouldShow)
	{
		bShouldShowLoadingScreen = bShouldShow;
	}
	
	FString FlowId = TEXT("DefaultFlow");
	FString DefaultLoadingReason = TEXT("Flow Pending...");

private:
	TSharedPtr<FControlFlow> ActiveFlow;
	uint8 bShouldShowLoadingScreen : 1 = true; // TFunction에서는 이 변수를 직접 접근하면 주소가 달라져서 따로 함수를 통해 접근해야 함...
};
