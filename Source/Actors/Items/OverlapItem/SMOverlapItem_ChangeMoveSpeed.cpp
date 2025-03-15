// Copyright Studio Surround. All Rights Reserved.

#include "SMOverlapItem_ChangeMoveSpeed.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMCharacterMovementComponent.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "Utilities/SMLog.h"

void ASMOverlapItem_ChangeMoveSpeed::ActivateItem(AActor* InActivator)
{
	USMAbilitySystemComponent* ActivatorASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(InActivator);
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(InActivator);
	USMCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement<USMCharacterMovementComponent>() : nullptr;
	if (!ActivatorASC || !SourceCharacter || !SourceMovement)
	{
		return;
	}

	const FGameplayEffectSpecHandle GESpec = ActivatorASC->MakeOutgoingSpec(GE, 1.0f, ActivatorASC->MakeEffectContext());
	if (GESpec.IsValid())
	{
		NET_VLOG(this, INDEX_NONE, Duration < 0.0f ? 3.0f : Duration, TEXT("%s 버프 적용 중"), *GetName());
		GESpec.Data->SetSetByCallerMagnitude(SMTags::Data::Duration, Duration);
		const FActiveGameplayEffectHandle NewBuffHandle = ActivatorASC->BP_ApplyGameplayEffectSpecToSelf(GESpec);

		SourceCharacter->SetLastAppliedBuffAndGCTag(NewBuffHandle, SMTags::GameplayCue::Common::Item::MoveSpeedUp);
		SourceMovement->ClientAddMoveSpeedBuff(MoveSpeedMultiplier, Duration);
	}

	Super::ActivateItem(InActivator);
}
