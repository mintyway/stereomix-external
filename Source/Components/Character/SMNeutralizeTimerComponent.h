// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMNeutralizeTimerComponent.generated.h"


class ASMPlayerCharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMNeutralizeTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMNeutralizeTimerComponent();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartTimer();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEndTimer();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetNeutralizedTimer(float NewTime);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetMinimalNeutralizeTimer(float NewTime);

	TMulticastDelegate<void(float)> OnNeutralizeRemainTimeRatio;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCurrentNeutralizedRemainTimeRatio(float NewRatio);

	float MaxNeutralizeTime = 0.0f;

	float CurrentNeutralizeTime = 0.0f;

	float MaxMinimalNeutralizeTime = 0.0f;

	float CurrentMinimalNeutralizeTime = 0.0f;

	float CurrentNeutralizeRemainTimeRatio = 0.0f;
};
