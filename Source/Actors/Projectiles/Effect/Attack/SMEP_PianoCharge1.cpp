// Copyright Studio Surround. All Rights Reserved.


#include "SMEP_PianoCharge1.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Utilities/SMLog.h"


void ASMEP_PianoCharge1::AddProjectileFX()
{
	AActor* SourceActor = GetOwner();
	if (USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = this;
		GCParams.TargetAttachComponent = GetRootComponent();
		SourceASC->AddGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge1, GCParams);
	}
}

void ASMEP_PianoCharge1::RemoveProjectileFX()
{
	AActor* SourceActor = GetOwner();
	if (USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = this;
		SourceASC->RemoveGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryProjectileCharge1, GCParams);
	}
}

void ASMEP_PianoCharge1::PlayHitFX(AActor* InTarget)
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
	SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryHitCharge1, GCParams);
}

void ASMEP_PianoCharge1::PlayWallHitFX(const FVector& HitLocation)
{
	AActor* SourceActor = GetOwner();
	if (const USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)))
	{
		FGameplayCueParameters GCParams;
		GCParams.Location = HitLocation;
		GCParams.Normal = GetActorRotation().Vector();
		SourceASC->ExecuteGC(SourceActor, SMTags::GameplayCue::Piano::ArcheryWallHitCharge1, GCParams);
	}
}
