// Copyright Studio Surround. All Rights Reserved.


#include "SMPerformanceSettings.h"
#include "SMPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPerformanceSettings)

USMPlatformSpecificRenderingSettings::USMPlatformSpecificRenderingSettings()
{
}

const USMPlatformSpecificRenderingSettings& USMPlatformSpecificRenderingSettings::Get()
{
	USMPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return *Result;
}

USMPerformanceSettings::USMPerformanceSettings()
{
	PerPlatformSettings.Initialize(USMPlatformSpecificRenderingSettings::StaticClass());
	CategoryName = TEXT("Game");
	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	FSMPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (ESMDisplayablePerformanceStat PerfStat : TEnumRange<ESMDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}
