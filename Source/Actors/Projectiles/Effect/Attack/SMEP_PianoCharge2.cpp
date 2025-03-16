// Copyright Studio Surround. All Rights Reserved.


#include "SMEP_PianoCharge2.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Data/DataAsset/Character/SMPianoCharacterDataAsset.h"
#include "Utilities/SMLog.h"

class ASMPlayerCharacterBase;

void ASMEP_PianoCharge2::AddProjectileFX()
{
	AActor* SourceActor = GetOwner();
	if (USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = this;
		GCParams.TargetAttachComponent = GetRootComponent();
		SourceASC->AddGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge2, GCParams);
	}
}

void ASMEP_PianoCharge2::RemoveProjectileFX()
{
	AActor* SourceActor = GetOwner();
	if (USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = this;
		SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge2, GCParams);
	}
}

void ASMEP_PianoCharge2::PlayHitFX(AActor* InTarget)
{
	AActor* SourceActor = GetOwner();
	const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (!InTarget || !SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.Normal = GetActorRotation().Vector();
	GCParams.TargetAttachComponent = InTarget->GetRootComponent();
	SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryHitCharge2, GCParams);
}

void ASMEP_PianoCharge2::PlayWallHitFX(const FVector& HitLocation)
{
	AActor* SourceActor = GetOwner();
	const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
	if (!SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.Location = HitLocation;
	GCParams.Normal = GetActorRotation().Vector();
	SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryWallHitCharge2, GCParams);
}
