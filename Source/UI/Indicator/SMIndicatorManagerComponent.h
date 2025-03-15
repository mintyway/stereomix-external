// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ControllerComponent.h"

#include "SMIndicatorManagerComponent.generated.h"

class UIndicatorDescriptor;

UCLASS(BlueprintType, Blueprintable)
class STEREOMIX_API USMIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	USMIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static USMIndicatorManagerComponent* GetComponent(const AController* Controller);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(USMIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor*);

	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};
