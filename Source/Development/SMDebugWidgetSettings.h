// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/Debug/SMGameVersionWidget.h"
#include "UI/PerformanceStats/SMPerfStatContainer.h"
#include "SMDebugWidgetSettings.generated.h"


UCLASS(Config = Game, DefaultConfig, DisplayName = "StereoMix Debug Widget Settings")
class STEREOMIX_API USMDebugWidgetSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Debug Widget")
	TSoftClassPtr<USMGameVersionWidget> GameVersionWidgetClass;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Debug Widget")
	TSoftClassPtr<USMPerfStatContainer> NetworkPerformanceStatContainerClass;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Debug Widget")
	TSoftClassPtr<USMPerfStatContainer> StandalonePerformanceStatContainerClass;
};
