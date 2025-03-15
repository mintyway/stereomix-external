// Copyright Surround, Inc. All Rights Reserved.


#include "SMElectricGuitarCharacter.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/Character/SMEmoticonWidgetComponent.h"
#include "Components/Character/SMFaceControlComponent.h"
#include "Components/Character/SMHIC_Character.h"


ASMElectricGuitarCharacter::ASMElectricGuitarCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterType = ESMCharacterType::ElectricGuitar;

	FaceControlComponent->InEyeMaterialIndex = 3;
	FaceControlComponent->OutEyeMaterialIndex = 1;

	constexpr double NameplateHeight = 125.0;
	NameplateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, NameplateHeight));
	EmoticonWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, NameplateHeight + 200.0));
}

void ASMElectricGuitarCharacter::OnHoldStateEntry()
{
	Super::OnHoldStateEntry();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = this;
			GCParams.RawMagnitude = Cast<ASMCharacterBase>(HIC->GetActorIAmHolding()) ? 0.0f : 1.0f;
			SourceASC->AddGameplayCue(SMTags::GameplayCue::ElectricGuitar::HoldWeapon, GCParams);
		}
	}
}

void ASMElectricGuitarCharacter::OnHoldStateExit()
{
	Super::OnHoldStateExit();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			SourceASC->RemoveGameplayCue(SMTags::GameplayCue::ElectricGuitar::HoldWeapon);
		}
	}
}
