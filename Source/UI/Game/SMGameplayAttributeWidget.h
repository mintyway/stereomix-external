// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CommonUserWidget.h"
#include "GameplayEffectTypes.h"
#include "SMGameplayAbilitySystemWidget.h"
#include "SMGameplayAttributeWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSMOnAttributeValueChanged, const FGameplayAttribute& ChangedAttribute, float OldValue, float NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSMOnAttributeValueChangedDynamic, const FGameplayAttribute&, ChangedAttribute, float, OldValue, float, NewValue);

UCLASS(Abstract, DisplayName = "StereoMix Gameplay Attribute Widget")
class STEREOMIX_API USMGameplayAttributeWidget : public USMGameplayAbilitySystemWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnRegisterASC(USMAbilitySystemComponent* InOwningASC) override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintPure, Category = "Gameplay Attribute")
	float GetAttributeValue() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay Attribute")
	float GetAttributeMaxValue() const;

	FSMOnAttributeValueChanged OnAttributeValueChangedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Attribute", DisplayName = "On Attribute Value Changed")
	FSMOnAttributeValueChangedDynamic K2_OnAttributeValueChangedEvent;

protected:
	void OnAttributeValueChanged(const FOnAttributeChangeData& AttributeChangeData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay Attribute", DisplayName = "On Attribute Value Changed")
	void BP_OnAttributeValueChanged(const FGameplayAttribute& ChangedAttribute, float OldValue, float NewValue);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Attribute")
	FGameplayAttribute AttributeMax;
	
	FDelegateHandle AttributeValueChangedEventHandle;
	FDelegateHandle AttributeMaxValueChangedEventHandle;
};
