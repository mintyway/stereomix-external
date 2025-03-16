// Copyright Surround, Inc. All Rights Reserved.


#include "SMPianoCharacter.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Weapons/SMBow.h"
#include "Components/Character/SMEmoticonWidgetComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Utilities/SMLog.h"


ASMPianoCharacter::ASMPianoCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CharacterType = ESMCharacterType::Piano;

	constexpr double NameplateHeight = 150.0;
	NameplateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, NameplateHeight));
	EmoticonWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, NameplateHeight + 200.0));
}

void ASMPianoCharacter::SetWeaponVFXEnabled(bool bNewIsEnabled)
{
	if (ASMBow* Bow = GetWeapon<ASMBow>())
	{
		Bow->SetVFXEnabled(bNewIsEnabled);
	}
}

void ASMPianoCharacter::OnHoldStateEntry()
{
	Super::OnHoldStateEntry();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = this;
			GCParams.RawMagnitude = Cast<ASMCharacterBase>(HIC->GetActorIAmHolding()) ? 0.0f : 1.0f;
			SourceASC->AddGameplayCue(SMTags::GameplayCue::Piano::HoldWeapon, GCParams);
		}
	}
}

void ASMPianoCharacter::OnHoldStateExit()
{
	Super::OnHoldStateExit();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			SourceASC->RemoveGameplayCue(SMTags::GameplayCue::Piano::HoldWeapon);
		}
	}
}
