// Copyright Studio Surround. All Rights Reserved.


#include "SMAT_BassNoiseBreakAdditionalEffects.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "AbilitySystem/Abilities/NoiseBreak/SMGA_BassNoiseBreak.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Components/Character/SMHIC_Character.h"
#include "Utilities/SMLog.h"


USMAT_BassNoiseBreakAdditionalEffects* USMAT_BassNoiseBreakAdditionalEffects::BassNoiseBreakAdditionalEffects(UGameplayAbility* OwningAbility)
{
	USMAT_BassNoiseBreakAdditionalEffects* NewObj = NewAbilityTask<USMAT_BassNoiseBreakAdditionalEffects>(OwningAbility);
	NewObj->OwningAbility = Cast<USMGA_BassNoiseBreak>(OwningAbility);
	return NewObj;
}

void USMAT_BassNoiseBreakAdditionalEffects::Activate()
{
	if (!OwningAbility || !OwningAbility->IsLocallyControlled())
	{
		EndTask();
		return;
	}

	UAbilityTask_WaitGameplayEvent* WeaponTrailActivationTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(OwningAbility, SMTags::Event::AnimNotify::Bass::NoiseBreakWeaponTrailActivate);
	WeaponTrailActivationTask->EventReceived.AddDynamic(this, &ThisClass::OnWeaponTrailActivate);
	WeaponTrailActivationTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WeaponTrailDeactivationTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(OwningAbility, SMTags::Event::AnimNotify::Bass::NoiseBreakWeaponTrailDeactivate);
	WeaponTrailDeactivationTask->EventReceived.AddDynamic(this, &ThisClass::OnWeaponTrailDeactivate);
	WeaponTrailDeactivationTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* SlashTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(OwningAbility, SMTags::Event::AnimNotify::Bass::NoiseBreakSlash);
	SlashTask->EventReceived.AddDynamic(this, &ThisClass::OnSlash);
	SlashTask->ReadyForActivation();
}

void USMAT_BassNoiseBreakAdditionalEffects::OnWeaponTrailActivate(FGameplayEventData Payload)
{
	if (!OwningAbility)
	{
		EndTask();
		return;
	}

	ASMPlayerCharacterBase* SourceCharacter = OwningAbility->GetCharacter();
	const USMAbilitySystemComponent* SourceASC = OwningAbility->GetASC();
	const USMHIC_Character* SourceHIC = OwningAbility->GetHIC();
	const ASMWeaponBase* SourceWeapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceHIC || !SourceWeaponMesh)
	{
		return;
	}

	const FVector WeaponOffset(30.0, 0.0, -145.0);

	const auto TargetActor = SourceHIC->GetActorIAmHolding();
	const float HoldingType = TargetActor ? (Cast<ASMCharacterBase>(TargetActor) ? 0.0f : 1.0f) : 0.0f; // 0이면 캐릭터, 1이면 아이템입니다.

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.TargetAttachComponent = SourceWeaponMesh;
	GCParams.Location = WeaponOffset;
	GCParams.RawMagnitude = HoldingType;
	SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakWeaponTrail, GCParams);
}

void USMAT_BassNoiseBreakAdditionalEffects::OnWeaponTrailDeactivate(FGameplayEventData Payload)
{
	if (!OwningAbility)
	{
		EndTask();
		return;
	}

	ASMPlayerCharacterBase* SourceCharacter = OwningAbility->GetCharacter();
	const USMAbilitySystemComponent* SourceASC = OwningAbility->GetASC();
	const USMHIC_Character* SourceHIC = OwningAbility->GetHIC();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakWeaponTrail, GCParams);
}

void USMAT_BassNoiseBreakAdditionalEffects::OnSlash(FGameplayEventData Payload)
{
	if (!OwningAbility)
	{
		EndTask();
		return;
	}

	ASMPlayerCharacterBase* SourceCharacter = OwningAbility->GetCharacter();
	const USMAbilitySystemComponent* SourceASC = OwningAbility->GetASC();
	const USMHIC_Character* SourceHIC = OwningAbility->GetHIC();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		return;
	}

	const auto TargetActor = SourceHIC->GetActorIAmHolding();
	const float HoldingType = TargetActor ? (Cast<ASMCharacterBase>(TargetActor) ? 0.0f : 1.0f) : 0.0f; // 0이면 캐릭터, 1이면 아이템입니다.

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	GCParams.Location = OwningAbility->GetNoiseBreakTargetLocation();
	GCParams.Normal = SourceCharacter->GetActorRotation().Vector();
	GCParams.RawMagnitude = HoldingType;
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::NoiseBreakSlash, GCParams);
}
