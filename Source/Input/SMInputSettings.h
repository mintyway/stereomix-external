// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "InputMappingTypes.h"
#include "SMInputSettings.generated.h"

/**
 * StereoMix Input Settings
 */
UCLASS(Config = Game, DefaultConfig, DisplayName = "StereoMix Input Settings")
class STEREOMIX_API USMInputSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Input", meta = (DisplayName = "Default Player Input Mapping Contexts"))
	TArray<FSMInputMappingContextSetting> DefaultMappingContexts;
};
