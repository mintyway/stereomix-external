// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SMWorldLoadingSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FSMOnLevelLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSMOnLevelLoadedDynamic);

UCLASS()
class STEREOMIX_API USMWorldLoadingSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual void UpdateStreamingState() override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|WorldLoading")
	bool IsLevelLoaded() const { return bLevelLoaded; }

	FSMOnLevelLoaded OnLevelLoadedEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|WorldLoading", meta = (DisplayName = "On Level Loaded"))
	FSMOnLevelLoadedDynamic K2_OnLevelLoadedEvent;

private:
	bool bLevelLoaded = false;
};
