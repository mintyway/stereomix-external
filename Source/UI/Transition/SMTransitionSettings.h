// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMTransitionScreen.h"
#include "SMTransitionSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, DisplayName="StereoMix Transition Settings")
class STEREOMIX_API USMTransitionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USMTransitionSettings();

	UPROPERTY(Config, EditAnywhere, Category=Display)
	TSoftClassPtr<USMTransitionScreen> DefaultTransitionScreenClass;

	UPROPERTY(Config, EditAnywhere, Category=Display)
	int32 TransitionScreenZOrder = 1000;
};
