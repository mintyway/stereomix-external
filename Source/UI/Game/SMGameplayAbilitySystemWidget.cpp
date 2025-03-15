// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayAbilitySystemWidget.h"

void USMGameplayAbilitySystemWidget::RegisterASC(USMAbilitySystemComponent* InOwningASC)
{
	if (ensure(InOwningASC))
	{
		OwningAbilitySystemComponent = InOwningASC;
		NativeOnRegisterASC(InOwningASC);
		BP_OnRegisterASC(InOwningASC);
	}
}

void USMGameplayAbilitySystemWidget::NativeOnRegisterASC(USMAbilitySystemComponent* InOwningASC)
{
}

USMAbilitySystemComponent* USMGameplayAbilitySystemWidget::GetOwningAbilitySystemComponent() const
{
	return OwningAbilitySystemComponent.Get();
}
