// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMGameplayPopWidget.generated.h"

class USMGameplayPopDescriptor;
class USMGameplayPopLayer;

UCLASS(Abstract)
class STEREOMIX_API USMGameplayPopWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void ActivateGameplayPop(const USMGameplayPopDescriptor* InDescriptor);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Pop")
	void FinishPlay();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Pop")
	void PlayPopAnimation(UWidgetAnimation* Animation);

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	FSimpleDelegate OnFinishPlay;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay Pop", DisplayName = "On Activate Pop")
	void BP_OnActivatePop(const USMGameplayPopDescriptor* InDescriptor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay Pop", DisplayName = "On Deactivate Pop")
	void BP_OnDeactivatePop();

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> CurrentAnimation;
};
