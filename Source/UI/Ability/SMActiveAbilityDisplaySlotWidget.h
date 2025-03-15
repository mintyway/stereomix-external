// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMActiveAbilityDisplaySlotData.h"
#include "SMActiveAbilityDisplaySlotWidget.generated.h"

class UWidgetSwitcher;
class USMActiveAbilityDisplayEntryWidget;
class UCommonVisibilitySwitcher;
class USMAbilitySystemComponent;
struct FSMGameplayAbilitySlotMessage;
class USMDisplayableGameplayAbility;
enum class ESMAbilityDisplayState : uint8;


UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName = "StereoMix Active Ability Display Slot Widget")
class STEREOMIX_API USMActiveAbilityDisplaySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "AbilityDisplay")
	void SetData(const USMActiveAbilityDisplaySlotData* InSlotData);

	UFUNCTION(BlueprintPure, Category = "AbilityDisplay")
	USMAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateState();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityDisplay")
	TSubclassOf<USMActiveAbilityDisplayEntryWidget> EntryClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "AbilityDisplay")
	TObjectPtr<const USMActiveAbilityDisplaySlotData> SlotData;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> EntrySlot;

	UPROPERTY()
	TObjectPtr<USMAbilitySystemComponent> AbilitySystemComponent;

	TArray<TObjectPtr<USMActiveAbilityDisplayEntryWidget>> EntryWidgets;

	bool bIsInitialized = false;
};
