// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "Engine/PlatformSettings.h"
#include "GameplayTagContainer.h"
#include "SMPerformanceSettings.generated.h"

enum class ESMDisplayablePerformanceStat : uint8;
class UObject;

// Describes one platform-specific device profile variant that the user can choose from in the UI
USTRUCT()
struct FSMQualityDeviceProfileVariant
{
	GENERATED_BODY()

	// The display name for this device profile variant (visible in the options screen)
	UPROPERTY(EditAnywhere)
	FText DisplayName;

	// The suffix to append to the base device profile name for the current platform
	UPROPERTY(EditAnywhere)
	FString DeviceProfileSuffix;

	// The minimum required refresh rate to enable this mode
	// (e.g., if this is set to 120 Hz and the device is connected
	// to a 60 Hz display, it won't be available)
	UPROPERTY(EditAnywhere)
	int32 MinRefreshRate = 0;
};

// Describes a set of performance stats that the user can enable in settings,
// predicated on passing a visibility query on platform traits
USTRUCT()
struct FSMPerformanceStatGroup
{
	GENERATED_BODY()

	// A query on platform traits to determine whether or not it will be possible
	// to show a set of stats
	UPROPERTY(EditAnywhere, meta=(Categories = "Input,Platform.Trait"))
	FGameplayTagQuery VisibilityQuery;

	// The set of stats to allow if the query passes
	UPROPERTY(EditAnywhere)
	TSet<ESMDisplayablePerformanceStat> AllowedStats;
};

// How hare frame pacing and overall graphics settings controlled/exposed for the platform?
UENUM()
enum class ESMFramePacingMode : uint8
{
	// Manual frame rate limits, user is allowed to choose whether or not to lock to vsync
	DesktopStyle,

	// Limits handled by choosing present intervals driven by device profiles
	ConsoleStyle,

	// Limits handled by a user-facing choice of frame rate from among ones allowed by device profiles for the specific device
	MobileStyle
};

UCLASS(Config=Game, DefaultConfig)
class STEREOMIX_API USMPlatformSpecificRenderingSettings : public UPlatformSettings
{
	GENERATED_BODY()

public:
	USMPlatformSpecificRenderingSettings();

	static const USMPlatformSpecificRenderingSettings& Get();

	UPROPERTY(EditAnywhere, Config, Category=DeviceProfiles)
	FString DefaultDeviceProfileSuffix;

	/**
	 * 유저가 설정에서 선택할 수 있는 디바이스 프로필 변형 목록
	 *
	 * 프레임레이트가 느린 순으로 정렬되어야 히며 플랫폼 별 ini파일을 통해 설정됩니다.
	 */
	UPROPERTY(EditAnywhere, Config, Category=DeviceProfiles)
	TArray<FSMQualityDeviceProfileVariant> UserFacingDeviceProfileOptions;

	UPROPERTY(EditAnywhere, Config, Category=VideoSettings)
	bool bSupportsAutomaticVideoQualityBenchmark = true;

	UPROPERTY(EditAnywhere, Config, Category=VideoSettings)
	ESMFramePacingMode FramePacingMode = ESMFramePacingMode::DesktopStyle;
};

/**
 * 성능 프로파일 설정
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="StereoMix Performance Settings"))
class STEREOMIX_API USMPerformanceSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	USMPerformanceSettings();

	/**
	 * 유저가 선택 가능한 프레임 레이트 목록입니다.
	 */
	UPROPERTY(EditAnywhere, Config, Category=Performce, meta=(ForceUnits=Hz))
	TArray<int32> DesktopFrameRateLimits;

	/**
	 * 유저에 의해 활성화 가능한 퍼포먼스 스탯 그룹 목록입니다.
	 */
	UPROPERTY(EditAnywhere, Config, Category=Stats)
	TArray<FSMPerformanceStatGroup> UserFacingPerformanceStats;
	
private:
	UPROPERTY(EditAnywhere, Category=PlatformSpecific)
	FPerPlatformSettings PerPlatformSettings;
};
