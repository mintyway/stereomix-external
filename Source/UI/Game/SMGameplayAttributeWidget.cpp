// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayAttributeWidget.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"


void USMGameplayAttributeWidget::NativeOnRegisterASC(USMAbilitySystemComponent* InOwningASC)
{
	Super::NativeOnRegisterASC(InOwningASC);
	if (InOwningASC)
	{
		AttributeValueChangedEventHandle = InOwningASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::OnAttributeValueChanged);
		AttributeMaxValueChangedEventHandle = InOwningASC->GetGameplayAttributeValueChangeDelegate(AttributeMax).AddUObject(this, &ThisClass::OnAttributeValueChanged);
	}
}

void USMGameplayAttributeWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (USMAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		if (AttributeValueChangedEventHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(AttributeValueChangedEventHandle);
		}
		if (AttributeMaxValueChangedEventHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(AttributeMax).Remove(AttributeMaxValueChangedEventHandle);
		}
	}
}

float USMGameplayAttributeWidget::GetAttributeValue() const
{
	if (USMAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		return ASC->GetNumericAttribute(Attribute);
	}
	return 0.0f;
}

float USMGameplayAttributeWidget::GetAttributeMaxValue() const
{
	if (USMAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		return ASC->GetNumericAttribute(AttributeMax);
	}
	return 0.0f;
}

void USMGameplayAttributeWidget::OnAttributeValueChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	BP_OnAttributeValueChanged(AttributeChangeData.Attribute, AttributeChangeData.OldValue, AttributeChangeData.NewValue);
	OnAttributeValueChangedEvent.Broadcast(AttributeChangeData.Attribute, AttributeChangeData.OldValue, AttributeChangeData.NewValue);
	K2_OnAttributeValueChangedEvent.Broadcast(AttributeChangeData.Attribute, AttributeChangeData.OldValue, AttributeChangeData.NewValue);
}
