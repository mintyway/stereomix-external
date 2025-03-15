// Copyright Surround, Inc. All Rights Reserved.


#include "SMBassCharacter.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/SMEmoticonWidgetComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Utilities/SMCollision.h"


ASMBassCharacter::ASMBassCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterType = ESMCharacterType::Bass;

	constexpr double NameplateHeight = 200.0;
	NameplateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, NameplateHeight));
	EmoticonWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, NameplateHeight + 200.0));

	SlashColliderRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlashColliderRootComponent"));
	SlashColliderRootComponent->SetupAttachment(RootComponent);

	SlashColliderComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SlashColliderComponent"));
	SlashColliderComponent->SetupAttachment(SlashColliderRootComponent);
	SlashColliderComponent->SetCollisionProfileName(SMCollisionProfileName::SlashCollider);
	SlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	constexpr double HalfDistance = 250.0;
	SlashColliderComponent->SetRelativeLocationAndRotation(FVector(HalfDistance, 0.0, 0.0), FRotator(-90.0, 0.0, 0.0));
	SlashColliderComponent->InitCapsuleSize(50.0f, HalfDistance);

	LockAimTags.AddTag(SMTags::Character::State::Bass::Charge);
	LockAimTags.AddTag(SMTags::Ability::Activation::Slash);

	LockMovementTags.AddTag(SMTags::Character::State::Bass::Charge);

	PushBackImmuneTags.AddTag(SMTags::Character::State::Bass::Charge);
}

void ASMBassCharacter::OnHoldStateEntry()
{
	Super::OnHoldStateEntry();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = this;
			GCParams.RawMagnitude = Cast<ASMCharacterBase>(HIC->GetActorIAmHolding()) ? 0.0f : 1.0f;
			SourceASC->AddGameplayCue(SMTags::GameplayCue::Bass::HoldWeapon, GCParams);
		}
	}
}

void ASMBassCharacter::OnHoldStateExit()
{
	Super::OnHoldStateExit();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			SourceASC->RemoveGameplayCue(SMTags::GameplayCue::Bass::HoldWeapon);
		}
	}
}
