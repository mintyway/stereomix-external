// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayStateAbility.h"
#include "SMGA_Sampling.generated.h"

class ASMTile;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Sampling : public USMGameplayStateAbility
{
	GENERATED_BODY()

public:
	USMGA_Sampling();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void OnUnderTileChanged(ASMTile* UnderTile);

	void ResetSamplingStepTimer();

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTagContainer DeactivateTags;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TickRate = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SamplingStepResetTime = 3.0f;

	int32 CurrentSamplingStep = 0;

	FTimerHandle SamplingStepTimerHandle;
};
