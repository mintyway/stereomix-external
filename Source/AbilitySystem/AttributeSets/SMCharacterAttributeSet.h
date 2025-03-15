// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SMAttributeSet.h"
#include "Utilities/SMAttributeAccessor.h"

#include "SMCharacterAttributeSet.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMCharacterAttributeSet : public USMAttributeSet
{
	GENERATED_BODY()

public:
	USMCharacterAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, PostureGauge);
	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, MaxPostureGauge);
	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, MoveSpeed);
	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, MaxStamina);
	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, SkillGauge);
	ATTRIBUTE_ACCESSORS_REPLICATE(USMCharacterAttributeSet, MaxSkillGauge);

	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, Heal);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, HealSkillGauge);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, HealStamina);

	float GetBaseMoveSpeed() const { return MoveSpeed.GetBaseValue(); }

protected:
	UFUNCTION()
	void OnRep_PostureGauge(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxPostureGauge(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_SkillGauge(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxSkillGauge(const FGameplayAttributeData& OldValue) const;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_PostureGauge")
	FGameplayAttributeData PostureGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_MaxPostureGauge")
	FGameplayAttributeData MaxPostureGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_MoveSpeed")
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = "OnRep_Stamina")
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = "OnRep_MaxStamina")
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, Category = "SkillGauge", ReplicatedUsing = "OnRep_SkillGauge")
	FGameplayAttributeData SkillGauge;

	UPROPERTY(BlueprintReadOnly, Category = "SkillGauge", ReplicatedUsing = "OnRep_MaxSkillGauge")
	FGameplayAttributeData MaxSkillGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Heal")
	FGameplayAttributeData Heal;

	UPROPERTY(BlueprintReadOnly, Category = "Heal")
	FGameplayAttributeData HealSkillGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Heal")
	FGameplayAttributeData HealStamina;

	FGameplayTagContainer InvincibleStateTags;

	FGameplayTagContainer UnlimitStaminaTags;
};
