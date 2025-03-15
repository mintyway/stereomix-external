// Copyright Surround, Inc. All Rights Reserved.


#include "SMNameplateWidget.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"

void USMNameplateWidget::RegisterCharacter(ASMCharacterBase* InOwningCharacter)
{
	if (!InOwningCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("InOwningCharacter이 유효하지 않습니다."));
		return;
	}
	OwningCharacter = InOwningCharacter;
	BP_RegisterCharacter(InOwningCharacter);
}

ASMCharacterBase* USMNameplateWidget::GetOwningCharacter() const
{
	return OwningCharacter.Get();
}

ASMPlayerCharacterBase* USMNameplateWidget::GetOwningPlayerCharacter() const
{
	return Cast<ASMPlayerCharacterBase>(OwningCharacter);
}

const USMCharacterAttributeSet* USMNameplateWidget::GetOwningCharacterAttributeSet() const
{
	return GetOwningAbilitySystemComponent()->GetSet<USMCharacterAttributeSet>();
}

USMAbilitySystemComponent* USMNameplateWidget::GetOwningAbilitySystemComponent() const
{
	return GetOwningPlayerCharacter()->GetStereoMixAbilitySystemComponent();
}
