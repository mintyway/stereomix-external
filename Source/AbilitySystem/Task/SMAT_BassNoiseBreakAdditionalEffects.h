// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_BassNoiseBreakAdditionalEffects.generated.h"

class USMGA_BassNoiseBreak;
class ASMPlayerCharacterBase;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_BassNoiseBreakAdditionalEffects : public UAbilityTask
{
	GENERATED_BODY()

public:
	static USMAT_BassNoiseBreakAdditionalEffects* BassNoiseBreakAdditionalEffects(UGameplayAbility* OwningAbility);

protected:
	virtual void Activate() override;

	UPROPERTY()
	TObjectPtr<USMGA_BassNoiseBreak> OwningAbility;

private:
	UFUNCTION()
	void OnWeaponTrailActivate(FGameplayEventData Payload);

	UFUNCTION()
	void OnWeaponTrailDeactivate(FGameplayEventData Payload);

	UFUNCTION()
	void OnSlash(FGameplayEventData Payload);
};
