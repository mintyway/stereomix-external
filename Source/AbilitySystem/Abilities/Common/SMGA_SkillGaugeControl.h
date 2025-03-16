// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_SkillGaugeControl.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_SkillGaugeControl : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_SkillGaugeControl();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnTeamTileEntry();

	void OnTeamTileExit();

	FActiveGameplayEffectHandle SkillGaugeGEHandle;
};
