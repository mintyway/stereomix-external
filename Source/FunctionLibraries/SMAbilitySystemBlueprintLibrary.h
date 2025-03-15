// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "SMAbilitySystemBlueprintLibrary.generated.h"

class USMAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAbilitySystemBlueprintLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Actor"))
	static USMAbilitySystemComponent* GetStereoMixAbilitySystem(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "StereoMix|AbilitySystem", meta = (WorldContext = "WorldContextObject"))
	static int32 GetInputIdFromAbility(UObject* WorldContextObject, const UGameplayAbility* Ability);
};
