// Copyright Studio Surround. All Rights Reserved.


#include "SMIndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"


USMIndicatorManagerComponent::USMIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

USMIndicatorManagerComponent* USMIndicatorManagerComponent::GetComponent(const AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<USMIndicatorManagerComponent>();
	}
	return nullptr;
}

void USMIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void USMIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);

		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
