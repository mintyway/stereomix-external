// Copyright Studio Surround. All Rights Reserved.


#include "SMNeutralizeTimerComponent.h"

#include "Utilities/SMLog.h"


USMNeutralizeTimerComponent::USMNeutralizeTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void USMNeutralizeTimerComponent::MulticastStartTimer_Implementation()
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		Activate(true);
	}
}

void USMNeutralizeTimerComponent::MulticastEndTimer_Implementation()
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		Deactivate();
	}
}

void USMNeutralizeTimerComponent::MulticastResetNeutralizedTimer_Implementation(float NewTime)
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		CurrentNeutralizeTime = NewTime;
		MaxNeutralizeTime = NewTime;
	}
}

void USMNeutralizeTimerComponent::MulticastResetMinimalNeutralizeTimer_Implementation(float NewTime)
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		CurrentMinimalNeutralizeTime = NewTime;
		MaxMinimalNeutralizeTime = NewTime;
	}
}

void USMNeutralizeTimerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentNeutralizeTime -= DeltaTime;
	CurrentMinimalNeutralizeTime -= DeltaTime;

	const float Ratio = FMath::Clamp(CurrentNeutralizeTime / MaxNeutralizeTime, 0.0f, 1.0f);
	const float MinimalRatio = FMath::Clamp(CurrentMinimalNeutralizeTime / MaxMinimalNeutralizeTime, 0.0f, 1.0f);
	SetCurrentNeutralizedRemainTimeRatio(FMath::Min(Ratio, MinimalRatio));
}

void USMNeutralizeTimerComponent::SetCurrentNeutralizedRemainTimeRatio(float NewRatio)
{
	CurrentNeutralizeRemainTimeRatio = NewRatio;
	OnNeutralizeRemainTimeRatio.Broadcast(NewRatio);
}
