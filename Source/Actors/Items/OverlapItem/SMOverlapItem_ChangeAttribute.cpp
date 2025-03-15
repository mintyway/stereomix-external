// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItem_ChangeAttribute.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "Utilities/SMLog.h"

void ASMOverlapItem_ChangeAttribute::ActivateItem(AActor* InActivator)
{
	USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(InActivator);
	ASMPlayerCharacterBase* PlayerCharacter = Cast<ASMPlayerCharacterBase>(InActivator);
	if (!ActivatorASC || !PlayerCharacter)
	{
		return;
	}

	if (!DataAsset)
	{
		return;
	}

	const FGameplayEffectSpecHandle GESpec = ActivatorASC->MakeOutgoingSpec(GE, 1.0f, ActivatorASC->MakeEffectContext());
	if (GESpec.IsValid())
	{
		NET_VLOG(this, INDEX_NONE, Magnitude < 0.0f ? 3.0f : Magnitude, TEXT("%s 버프 적용 중"), *GetName());
		GESpec.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
		const FActiveGameplayEffectHandle NewBuffHandle = ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);

		PlayerCharacter->SetLastAppliedBuffAndGCTag(NewBuffHandle, GCTag);
	}

	Super::ActivateItem(InActivator);
}
