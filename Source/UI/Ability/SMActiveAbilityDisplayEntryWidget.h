// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMActiveAbilityDisplaySlotData.h"
#include "SMActiveAbilityDisplaySlotWidget.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "Blueprint/UserWidget.h"
#include "SMActiveAbilityDisplayEntryWidget.generated.h"

enum class ESMAbilityDisplayState : uint8;
class USMActiveAbilityDisplaySlotWidget;
struct FSMActiveAbilityDisplayEntryData;
struct FGameplayAbilitySpec;

UENUM(BlueprintType)
enum class ESMAbilityDisplayState : uint8
{
	None,
	Idle,
	Activated,
	Deactivated,
	Cooldown,
	Hidden
};


UCLASS()
class STEREOMIX_API USMActiveAbilityDisplayEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USMActiveAbilityDisplayEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityDisplay")
	void SetEntryData(USMActiveAbilityDisplaySlotWidget* InEntrySlot, const FSMActiveAbilityDisplayEntryData& InEntryData);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityDisplay", DisplayName = "OnEntryInitialized")
	void BP_OnEntryInitialized();

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityDisplay", DisplayName = "On Ability Updated")
	void BP_OnAbilityUpdated(USMDisplayableGameplayAbility* NewAbility);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityDisplay", DisplayName = "OnUpdateState")
	void BP_OnUpdateState(ESMAbilityDisplayState OldState, ESMAbilityDisplayState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityDisplay", DisplayName = "OnUpdateGauge")
	void BP_OnUpdateGauge(float CurrentGauge, float MaxGauge);

	UFUNCTION(BlueprintCallable, Category = "AbilityDisplay")
	bool TryGetGauge(float& OutCurrentGauge, float& OutMaxGauge) const;

	UFUNCTION(BlueprintPure, Category = "AbilityDisplay")
	bool HasGauge() const;

	UFUNCTION(BlueprintPure, Category = "AbilityDisplay")
	const FSMActiveAbilityDisplayEntryData& GetEntryData() const;

	UFUNCTION(BlueprintPure, Category = "AbilityDisplay")
	ESMActiveAbilityType GetAbilityType() const;

	/** 어빌리티 인스턴스를 새로 업데이트하고 반환합니다. 반환된 인스턴스는 nullptr일 수도 있기 때문에 IsValid 체크를 해야 합니다. */
	UFUNCTION(BlueprintCallable, Category = "AbilityDisplay", DisplayName = "Refresh GA Instance")
	USMDisplayableGameplayAbility* RefreshAbilityInstance();

	/** 캐싱된 어빌리티 인스턴스를 반환합니다. 인스턴스는 nullptr일 수도 있습니다. */
	UFUNCTION(BlueprintPure, Category = "AbilityDisplay", DisplayName = "Get Cached GA Instance")
	USMDisplayableGameplayAbility* GetCachedAbilityInstance() const;

	UFUNCTION(BlueprintPure, Category = "AbilityDisplay")
	ESMAbilityDisplayState GetCurrentState() const;

	void SetState(ESMAbilityDisplayState NewState);
	bool TryUpdateState(ESMAbilityDisplayState& OutOldState, ESMAbilityDisplayState& OutNewState);
	
protected:
	void HandleNewState(const ESMAbilityDisplayState& OldState, const ESMAbilityDisplayState& NewState);
	
private:
	UPROPERTY()
	TObjectPtr<USMActiveAbilityDisplaySlotWidget> EntrySlot;

	UPROPERTY()
	TObjectPtr<USMDisplayableGameplayAbility> AbilityInstance;
	
	ESMAbilityDisplayState CurrentState;
	FSMActiveAbilityDisplayEntryData EntryData;
};
