// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "InputMappingContext.h"
#include "InputMappingTypes.generated.h"

USTRUCT()
struct FSMInputMappingContextSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	UPROPERTY(EditAnywhere, Category="Input")
	int32 Priority = 0;

	/** If true, then this IMC will be applied immediately on PlayerController initialized */
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	bool bAddImmediately = false;
	
	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};
