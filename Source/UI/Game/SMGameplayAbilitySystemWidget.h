// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMGameplayAbilitySystemWidget.generated.h"


class USMAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FSMOnRegisterASC, USMAbilitySystemComponent*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnRegisterASCDynamic, USMAbilitySystemComponent*, InOwningASC);

UCLASS(Abstract, DisplayName = "StereoMix GAS Widget")
class STEREOMIX_API USMGameplayAbilitySystemWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GAS", DisplayName = "Register ASC")
	void RegisterASC(USMAbilitySystemComponent* InOwningASC);

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS", DisplayName = "On Register ASC")
	void BP_OnRegisterASC(USMAbilitySystemComponent* InOwningASC);
	virtual void NativeOnRegisterASC(USMAbilitySystemComponent* InOwningASC);
	
	UFUNCTION(BlueprintPure, Category = "GAS", DisplayName = "Get Owning ASC")
	USMAbilitySystemComponent* GetOwningAbilitySystemComponent() const;

	FSMOnRegisterASC OnRegisterASCEvent;

	UPROPERTY(BlueprintAssignable, Category = "GAS", DisplayName = "On Register ASC")
	FSMOnRegisterASCDynamic K2_OnRegisterASCEvent;

private:
	UPROPERTY()
	TObjectPtr<USMAbilitySystemComponent> OwningAbilitySystemComponent;
};
