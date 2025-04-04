// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyBase.h"
#include "SMGCN_HitFlash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGCN_HitFlash : public USMGameplayCueNotifyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
};
