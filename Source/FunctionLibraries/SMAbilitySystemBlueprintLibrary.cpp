// Copyright Studio Surround. All Rights Reserved.


#include "SMAbilitySystemBlueprintLibrary.h"

#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"

USMAbilitySystemComponent* USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(AActor* Actor)
{
	return Cast<USMAbilitySystemComponent>(GetAbilitySystemComponent(Actor));
}

int32 USMAbilitySystemBlueprintLibrary::GetInputIdFromAbility(UObject* WorldContextObject, const UGameplayAbility* Ability)
{
	if (Ability)
	{
		return Ability->GetCurrentAbilitySpec()->InputID;
	}
	UE_LOG(LogStereoMixAbilitySystem, Warning, TEXT("GetInputIdFromAbility called with null Ability"));
	return -1;
}
