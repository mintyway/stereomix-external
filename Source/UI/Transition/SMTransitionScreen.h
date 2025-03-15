// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMTransitionManager.h"
#include "SMTransitionScreen.generated.h"


/**
 * 
 */
UCLASS(Abstract, DisplayName = "StereoMix Transition Screen", ClassGroup = "StereoMix UI")
class STEREOMIX_API USMTransitionScreen : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void StartTransition(ESMTransitionType TransitionType);

	UFUNCTION(BlueprintPure, Category = "Transition")
	bool IsTransitioning() const { return bIsTransitioning; }

	FSMTransitionDelegate& GetTransitionCompletedDelegate() { return TransitionCompletedDelegate; }
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Transition")
	void TransIn();

	UFUNCTION(BlueprintImplementableEvent, Category = "Transition")
	void TransOut();

	UFUNCTION(BlueprintCallable, Category = "Transition")
	void FinishTransition();

private:
	uint8 bIsTransitioning : 1 = false;

	UPROPERTY()
	ESMTransitionType LastTransitionType;
	
	FSMTransitionDelegate TransitionCompletedDelegate;
};
