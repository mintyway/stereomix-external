// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingsLocal.h"

#include "AudioModulationStatics.h"
#include "CommonInputSubsystem.h"
#include "CommonUISettings.h"
#include "FMODBlueprintStatics.h"
#include "GameplayTagContainer.h"
#include "ICommonUIModule.h"
#include "Audio/SMAudioSettings.h"
#include "Development/SMPlatformEmulationSettings.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "HAL/PlatformFramePacer.h"
#include "Performance/SMPerformanceSettings.h"
#include "Performance/SMPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingsLocal)


#if WITH_EDITOR
static TAutoConsoleVariable CVarApplyFrameRateSettingsInPIE(TEXT("StereoMix.Settings.ApplyFrameRateSettingsInPIE"),
	false,
	TEXT("Should we apply frame rate settings in PIE?"),
	ECVF_Default);

static TAutoConsoleVariable CVarApplyFrontEndPerformanceOptionsInPIE(TEXT("StereoMix.Settings.ApplyFrontEndPerformanceOptionsInPIE"),
	false,
	TEXT("Do we apply front-end specific performance options in PIE?"),
	ECVF_Default);

static TAutoConsoleVariable CVarApplyDeviceProfilesInPIE(TEXT("StereoMix.Settings.ApplyDeviceProfilesInPIE"),
	false,
	TEXT("Should we apply experience/platform emulated device profiles in PIE?"),
	ECVF_Default);
#endif

static TAutoConsoleVariable CVarDeviceProfileDrivenTargetFps(TEXT("StereoMix.DeviceProfile.Console.TargetFPS"),
	-1,
	TEXT("Target FPS when being driven by device profile"),
	ECVF_Default | ECVF_Preview);

static TAutoConsoleVariable CVarDeviceProfileDrivenFrameSyncType(TEXT("StereoMix.DeviceProfile.Console.FrameSyncType"),
	-1,
	TEXT("Sync type when being driven by device profile. Corresponds to r.GTSyncType"),
	ECVF_Default | ECVF_Preview);

FSMScalabilitySnapshot::FSMScalabilitySnapshot()
{
	static_assert(sizeof(Scalability::FQualityLevels) == 88, "This function may need to be updated to account for new members");

	Qualities.ResolutionQuality = -1.0f;
	Qualities.ViewDistanceQuality = -1;
	Qualities.AntiAliasingQuality = -1;
	Qualities.ShadowQuality = -1;
	Qualities.GlobalIlluminationQuality = -1;
	Qualities.ReflectionQuality = -1;
	Qualities.PostProcessQuality = -1;
	Qualities.TextureQuality = -1;
	Qualities.EffectsQuality = -1;
	Qualities.FoliageQuality = -1;
	Qualities.ShadingQuality = -1;
}

namespace StereoMix::SettingsHelpers
{
	bool HasPlatformTrait(FGameplayTag Tag)
	{
		return ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(Tag);
	}

	int32 GetHighestLevelOfAnyScalabilityChannel(const Scalability::FQualityLevels& ScalabilityQuality)
	{
		static_assert(sizeof(Scalability::FQualityLevels) == 88, "This function may need to be updated to account for new members");

		int32 MaxScalability = ScalabilityQuality.ViewDistanceQuality;
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.AntiAliasingQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.ShadowQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.GlobalIlluminationQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.ReflectionQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.PostProcessQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.TextureQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.EffectsQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.FoliageQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.ShadingQuality);

		return (MaxScalability >= 0) ? MaxScalability : -1;
	}

	void FillScalabilitySettingsFromDeviceProfile(FSMScalabilitySnapshot& Mode, const FString& Suffix = FString())
	{
		static_assert(sizeof(Scalability::FQualityLevels) == 88, "This function may need to be updated to account for new members");

		// Default out before filling so we can correctly mark non-overridden scalability values.
		// It's technically possible to swap device profile when testing so safest to clear and refill
		Mode = FSMScalabilitySnapshot();

		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ResolutionQuality%s"), *Suffix), Mode.Qualities.ResolutionQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ViewDistanceQuality%s"), *Suffix), Mode.Qualities.ViewDistanceQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.AntiAliasingQuality%s"), *Suffix), Mode.Qualities.AntiAliasingQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ShadowQuality%s"), *Suffix), Mode.Qualities.ShadowQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.GlobalIlluminationQuality%s"), *Suffix), Mode.Qualities.GlobalIlluminationQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ReflectionQuality%s"), *Suffix), Mode.Qualities.ReflectionQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.PostProcessQuality%s"), *Suffix), Mode.Qualities.PostProcessQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.TextureQuality%s"), *Suffix), Mode.Qualities.TextureQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.EffectsQuality%s"), *Suffix), Mode.Qualities.EffectsQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.FoliageQuality%s"), *Suffix), Mode.Qualities.FoliageQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ShadingQuality%s"), *Suffix), Mode.Qualities.ShadingQuality);
	}
}

USMSettingsLocal::USMSettingsLocal()
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && FSlateApplication::IsInitialized())
	{
		OnApplicationActivationStateChangedHandle =
			FSlateApplication::Get()
			.OnApplicationActivationStateChanged()
			.AddUObject(this, &USMSettingsLocal::OnAppActivationStateChanged);
		ThisClass::SetToDefaults();
	}
}

USMSettingsLocal* USMSettingsLocal::Get()
{
	return GEngine ? CastChecked<USMSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void USMSettingsLocal::BeginDestroy()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().Remove(OnApplicationActivationStateChangedHandle);
	}
	Super::BeginDestroy();
}

void USMSettingsLocal::SetToDefaults()
{
	Super::SetToDefaults();

	bFMODLoaded = false;

	const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();
	UserChosenDeviceProfileSuffix = PlatformSettings.DefaultDeviceProfileSuffix;
	DesiredUserChosenDeviceProfileSuffix = UserChosenDeviceProfileSuffix;

	FrameRateLimit_InMenu = 60.0f;
	FrameRateLimit_WhenBackground = 30.0f;
}

void USMSettingsLocal::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	// Console platforms use rhi.SyncInterval to limit framerate
	const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();
	if (PlatformSettings.FramePacingMode == ESMFramePacingMode::ConsoleStyle)
	{
		FrameRateLimit = 0.0f;
	}

	DesiredUserChosenDeviceProfileSuffix = UserChosenDeviceProfileSuffix;

	StereoMix::SettingsHelpers::FillScalabilitySettingsFromDeviceProfile(DeviceDefaultScalabilitySettings);

	PerfStatSettingsChangedEvent.Broadcast();
}

void USMSettingsLocal::ConfirmVideoMode()
{
	Super::ConfirmVideoMode();
}

// Combines two limits, always taking the minimum of the two (with special handling for values of <= 0 meaning unlimited)
float CombineFrameRateLimits(const float Limit1, const float Limit2)
{
	if (Limit1 <= 0.0f)
	{
		return Limit2;
	}
	if (Limit2 <= 0.0f)
	{
		return Limit1;
	}
	return FMath::Min(Limit1, Limit2);
}

float USMSettingsLocal::GetEffectiveFrameRateLimit()
{
	const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();

#if WITH_EDITOR
	if (GIsEditor && !CVarApplyFrameRateSettingsInPIE.GetValueOnGameThread())
	{
		return Super::GetEffectiveFrameRateLimit();
	}
#endif

	if (PlatformSettings.FramePacingMode == ESMFramePacingMode::ConsoleStyle)
	{
		return 0.0f;
	}

	float EffectiveFrameRateLimit = Super::GetEffectiveFrameRateLimit();

	if (ShouldUseFrontendPerformanceSettings())
	{
		EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_InMenu);
	}

	if (PlatformSettings.FramePacingMode == ESMFramePacingMode::DesktopStyle)
	{
		if (FSlateApplication::IsInitialized() && !FSlateApplication::Get().IsActive())
		{
			EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_WhenBackground);
		}
	}

	return EffectiveFrameRateLimit;
}

void USMSettingsLocal::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
	UserChosenDeviceProfileSuffix = DesiredUserChosenDeviceProfileSuffix;
}

void USMSettingsLocal::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

	if (!bSoundControlBusMixLoaded)
	{
		LoadUserControlBusMix();
	}
	{
		if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("Overall")))
		{
			if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
			{
				SetVolumeForControlBus(ControlBusPtr, OverallVolume);
			}
		}
		if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("Music")))
		{
			if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
			{
				SetVolumeForControlBus(ControlBusPtr, MusicVolume);
			}
		}
		if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("SoundFX")))
		{
			if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
			{
				SetVolumeForControlBus(ControlBusPtr, SoundFXVolume);
			}
		}
		if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("VoiceChat")))
		{
			if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
			{
				SetVolumeForControlBus(ControlBusPtr, VoiceChatVolume);
			}
		}
	}
	if (!bFMODLoaded)
	{
		LoadFMOD();
	}
	if (FMOD_MasterVCA)
	{
		SetVolumeForVCA(FMOD_MasterVCA, OverallVolume);
	}
	if (FMOD_MusicVCA)
	{
		SetVolumeForVCA(FMOD_MusicVCA, MusicVolume);
	}
	if (FMOD_SoundFxVCA)
	{
		SetVolumeForVCA(FMOD_SoundFxVCA, SoundFXVolume);
	}
	// TODO: VoiceChat 볼륨 설정

	if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetTypedOuter<ULocalPlayer>()))
	{
		InputSubsystem->SetGamepadInputType(ControllerPlatform);
	}

	if (DesiredUserChosenDeviceProfileSuffix != UserChosenDeviceProfileSuffix)
	{
		UserChosenDeviceProfileSuffix = DesiredUserChosenDeviceProfileSuffix;
	}

	if (FApp::CanEverRender())
	{
		ApplyDisplayGamma();
		UpdateGameModeDeviceProfileAndFps();
	}

	PerfStatSettingsChangedEvent.Broadcast();
}

int32 USMSettingsLocal::GetOverallScalabilityLevel() const
{
	int32 Result = Super::GetOverallScalabilityLevel();
	return Result;
}

void USMSettingsLocal::SetOverallScalabilityLevel(int32 Value)
{
	Super::SetOverallScalabilityLevel(Value);
}

void USMSettingsLocal::ApplyScalabilitySettings()
{
	SetQualityLevels(ScalabilityQuality);
}

void USMSettingsLocal::OnAppActivationStateChanged(bool bIsActive)
{
	// We might want to adjust the frame rate when the app loses/gains focus on multi-window platforms
	UpdateEffectiveFrameRateLimit();
}

void USMSettingsLocal::ReapplyThingsDueToPossibleDeviceProfileChange()
{
	ApplyNonResolutionSettings();
}

void USMSettingsLocal::SetShouldUseFrontendPerformanceSettings(bool bInFrontEnd)
{
	bInFrontEndForPerformancePurposes = bInFrontEnd;
	UpdateEffectiveFrameRateLimit();
}

bool USMSettingsLocal::ShouldUseFrontendPerformanceSettings() const
{
#if WITH_EDITOR
	if (GIsEditor && !CVarApplyFrontEndPerformanceOptionsInPIE.GetValueOnGameThread())
	{
		return false;
	}
#endif

	return bInFrontEndForPerformancePurposes;
}

ESMStatDisplayMode USMSettingsLocal::GetPerfStatDisplayState(ESMDisplayablePerformanceStat Stat) const
{
	if (const ESMStatDisplayMode* Mode = DisplayStatList.Find(Stat))
	{
		return *Mode;
	}
	return ESMStatDisplayMode::Hidden;
}

void USMSettingsLocal::SetPerfStatDisplayState(ESMDisplayablePerformanceStat Stat, ESMStatDisplayMode DisplayMode)
{
	if (DisplayMode == ESMStatDisplayMode::Hidden)
	{
		DisplayStatList.Remove(Stat);
	}
	else
	{
		DisplayStatList.FindOrAdd(Stat) = DisplayMode;
	}
	PerfStatSettingsChangedEvent.Broadcast();
}

float USMSettingsLocal::GetDisplayGamma() const
{
	return DisplayGamma;
}

void USMSettingsLocal::SetDisplayGamma(float InGamma)
{
	DisplayGamma = InGamma;
	ApplyDisplayGamma();
}

void USMSettingsLocal::ApplyDisplayGamma()
{
	if (GEngine)
	{
		GEngine->DisplayGamma = DisplayGamma;
	}
}

float USMSettingsLocal::GetFrameRateLimit_Always() const
{
	return GetFrameRateLimit();
}

void USMSettingsLocal::SetFrameRateLimit_Always(float NewLimitFPS)
{
	SetFrameRateLimit(NewLimitFPS);
	UpdateEffectiveFrameRateLimit();
}

float USMSettingsLocal::GetFrameRateLimit_InMenu() const
{
	return FrameRateLimit_InMenu;
}

void USMSettingsLocal::SetFrameRateLimit_InMenu(float NewLimitFPS)
{
	FrameRateLimit_InMenu = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

float USMSettingsLocal::GetFrameRateLimit_WhenBackground() const
{
	return FrameRateLimit_WhenBackground;
}

void USMSettingsLocal::SetFrameRateLimit_WhenBackground(float NewLimitFPS)
{
	FrameRateLimit_WhenBackground = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

void USMSettingsLocal::UpdateEffectiveFrameRateLimit()
{
	if (!IsRunningDedicatedServer())
	{
		SetFrameRateLimitCVar(GetEffectiveFrameRateLimit());
	}
}

FString USMSettingsLocal::GetDesiredDeviceProfileQualitySuffix() const
{
	return DesiredUserChosenDeviceProfileSuffix;
}

void USMSettingsLocal::SetDesiredDeviceProfileQualitySuffix(const FString& InDesiredSuffix)
{
	DesiredUserChosenDeviceProfileSuffix = InDesiredSuffix;
}

int32 USMSettingsLocal::GetMaxSupportedOverallQualityLevel() const
{
	const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();
	if ((PlatformSettings.FramePacingMode == ESMFramePacingMode::MobileStyle) && DeviceDefaultScalabilitySettings.bHasOverrides)
	{
		return StereoMix::SettingsHelpers::GetHighestLevelOfAnyScalabilityChannel(DeviceDefaultScalabilitySettings.Qualities);
	}
	return 3;
}

bool USMSettingsLocal::CanRunAutoBenchmark() const
{
	const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();
	return PlatformSettings.bSupportsAutomaticVideoQualityBenchmark;
}

bool USMSettingsLocal::ShouldRunAutoBenchmarkAtStartup() const
{
	if (!CanRunAutoBenchmark())
	{
		return false;
	}
	if (LastCPUBenchmarkResult != -1)
	{
		return false;
	}
	return true;
}

void USMSettingsLocal::RunAutoBenchmark(bool bSaveImmediately)
{
	RunHardwareBenchmark();

	// Always apply, optionally save
	ApplyScalabilitySettings();

	if (bSaveImmediately)
	{
		SaveSettings();
	}
}

void USMSettingsLocal::UpdateGameModeDeviceProfileAndFps()
{
#if WITH_EDITOR
	if (GIsEditor && !CVarApplyDeviceProfilesInPIE.GetValueOnGameThread())
	{
		return;
	}
#endif

	UDeviceProfileManager& Manager = UDeviceProfileManager::Get();

	const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();
	const TArray<FSMQualityDeviceProfileVariant>& UserFacingVariants = PlatformSettings.UserFacingDeviceProfileOptions;

	//@TODO: Might want to allow specific experiences to specify a suffix to attempt to use as well
	// The code below will handle searching with this suffix (alone or in conjunction with the frame rate), but nothing sets it right now
	FString ExperienceSuffix;

	// Make sure the chosen setting is supported for the current display, walking down the list to try fallbacks
	const int32 PlatformMaxRefreshRate = FPlatformMisc::GetMaxRefreshRate();

	int32 SuffixIndex = UserFacingVariants.IndexOfByPredicate([&](const FSMQualityDeviceProfileVariant& Data) { return Data.DeviceProfileSuffix == UserChosenDeviceProfileSuffix; });
	while (UserFacingVariants.IsValidIndex(SuffixIndex))
	{
		if (PlatformMaxRefreshRate >= UserFacingVariants[SuffixIndex].MinRefreshRate)
		{
			break;
		}
		--SuffixIndex;
	}

	const FString EffectiveUserSuffix = UserFacingVariants.IsValidIndex(SuffixIndex) ? UserFacingVariants[SuffixIndex].DeviceProfileSuffix : PlatformSettings.DefaultDeviceProfileSuffix;

	// Build up a list of names to try
	const bool bHadUserSuffix = !EffectiveUserSuffix.IsEmpty();
	const bool bHadExperienceSuffix = !ExperienceSuffix.IsEmpty();

	FString BasePlatformName = UDeviceProfileManager::GetPlatformDeviceProfileName();
	FName PlatformName; // Default unless in editor
#if WITH_EDITOR
	if (GIsEditor)
	{
		const USMPlatformEmulationSettings* Settings = GetDefault<USMPlatformEmulationSettings>();
		const FName PretendBaseDeviceProfile = Settings->GetPretendBaseDeviceProfile();
		if (PretendBaseDeviceProfile != NAME_None)
		{
			BasePlatformName = PretendBaseDeviceProfile.ToString();
		}

		PlatformName = Settings->GetPretendPlatformName();
	}
#endif

	TArray<FString> ComposedNamesToFind;
	if (bHadExperienceSuffix && bHadUserSuffix)
	{
		ComposedNamesToFind.Add(BasePlatformName + TEXT("_") + ExperienceSuffix + TEXT("_") + EffectiveUserSuffix);
	}
	if (bHadUserSuffix)
	{
		ComposedNamesToFind.Add(BasePlatformName + TEXT("_") + EffectiveUserSuffix);
	}
	if (bHadExperienceSuffix)
	{
		ComposedNamesToFind.Add(BasePlatformName + TEXT("_") + ExperienceSuffix);
	}
	if (GIsEditor)
	{
		ComposedNamesToFind.Add(BasePlatformName);
	}

	// See if any of the potential device profiles actually exists
	FString ActualProfileToApply;
	for (const FString& TestProfileName : ComposedNamesToFind)
	{
		if (Manager.HasLoadableProfileName(TestProfileName, PlatformName))
		{
			ActualProfileToApply = TestProfileName;
			UDeviceProfile* Profile = Manager.FindProfile(TestProfileName, /*bCreateOnFail=*/ false);
			if (Profile == nullptr)
			{
				Profile = Manager.CreateProfile(TestProfileName, TEXT(""), TestProfileName, *PlatformName.ToString());
			}

			UE_LOG(LogConsoleResponse, Log, TEXT("Profile %s exists"), *Profile->GetName());
			break;
		}
	}

	UE_LOG(LogConsoleResponse, Log, TEXT("UpdateGameModeDeviceProfileAndFps MaxRefreshRate=%d, ExperienceSuffix='%s', UserPicked='%s'->'%s', PlatformBase='%s', AppliedActual='%s'"),
		PlatformMaxRefreshRate, *ExperienceSuffix, *UserChosenDeviceProfileSuffix, *EffectiveUserSuffix, *BasePlatformName, *ActualProfileToApply);

	// Apply the device profile if it's different to what we currently have
	if (ActualProfileToApply != CurrentAppliedDeviceProfileOverrideSuffix)
	{
		if (Manager.GetActiveDeviceProfileName() != ActualProfileToApply)
		{
			// Restore the default first
			if (GIsEditor)
			{
#if ALLOW_OTHER_PLATFORM_CONFIG
				Manager.RestorePreviewDeviceProfile();
#endif
			}
			else
			{
				Manager.RestoreDefaultDeviceProfile();
			}

			// Apply the new one (if it wasn't the default)
			if (Manager.GetActiveDeviceProfileName() != ActualProfileToApply)
			{
				UDeviceProfile* NewDeviceProfile = Manager.FindProfile(ActualProfileToApply);
				ensureMsgf(NewDeviceProfile != nullptr, TEXT("DeviceProfile %s not found "), *ActualProfileToApply);
				if (NewDeviceProfile)
				{
					if (GIsEditor)
					{
#if ALLOW_OTHER_PLATFORM_CONFIG
						UE_LOG(LogConsoleResponse, Log, TEXT("Overriding *preview* device profile to %s"), *ActualProfileToApply);
						Manager.SetPreviewDeviceProfile(NewDeviceProfile);

						// Reload the default settings from the pretend profile
						StereoMix::SettingsHelpers::FillScalabilitySettingsFromDeviceProfile(DeviceDefaultScalabilitySettings);
#endif
					}
					else
					{
						UE_LOG(LogConsoleResponse, Log, TEXT("Overriding device profile to %s"), *ActualProfileToApply);
						Manager.SetOverrideDeviceProfile(NewDeviceProfile);
					}
				}
			}
		}
		CurrentAppliedDeviceProfileOverrideSuffix = ActualProfileToApply;
	}

	switch (PlatformSettings.FramePacingMode)
	{
		case ESMFramePacingMode::MobileStyle:
			UE_LOG(LogConsoleResponse, Warning, TEXT("UpdateGameModeDeviceProfileAndFps: MobileStyle is not supported."));
			break;
		case ESMFramePacingMode::ConsoleStyle:
			UpdateConsoleFramePacing();
			break;
		case ESMFramePacingMode::DesktopStyle:
			UpdateDesktopFramePacing();
			break;
	}
}

void USMSettingsLocal::UpdateConsoleFramePacing()
{
	// Apply device-profile-driven frame sync and frame pace
	const int32 FrameSyncType = CVarDeviceProfileDrivenFrameSyncType.GetValueOnGameThread();
	if (FrameSyncType != -1)
	{
		UE_LOG(LogConsoleResponse, Log, TEXT("Setting frame sync mode to %d."), FrameSyncType);
		SetSyncTypeCVar(FrameSyncType);
	}

	const int32 TargetFPS = CVarDeviceProfileDrivenTargetFps.GetValueOnGameThread();
	if (TargetFPS != -1)
	{
		UE_LOG(LogConsoleResponse, Log, TEXT("Setting frame pace to %d Hz."), TargetFPS);
		FPlatformRHIFramePacer::SetFramePace(TargetFPS);

		// Set the CSV metadata and analytics Fps mode strings
#if CSV_PROFILER
		const FString TargetFramerateString = FString::Printf(TEXT("%d"), TargetFPS);
		CSV_METADATA(TEXT("TargetFramerate"), *TargetFramerateString);
#endif
	}
}

void USMSettingsLocal::UpdateDesktopFramePacing()
{
	// For desktop the frame rate limit is handled by the parent class based on the value already
	// applied via UpdateEffectiveFrameRateLimit()
	// So this function is only doing 'second order' effects of desktop frame pacing preferences

	const float TargetFPS = GetEffectiveFrameRateLimit();
	const float ClampedFPS = TargetFPS <= 0.0f ? 60.0f : FMath::Clamp(TargetFPS, 30.0f, 60.0f);
	UpdateDynamicResFrameTime(ClampedFPS);
}

void USMSettingsLocal::UpdateDynamicResFrameTime(float TargetFPS)
{
	static IConsoleVariable* CVarDyResFrameTimeBudget = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.FrameTimeBudget"));
	if (CVarDyResFrameTimeBudget)
	{
		if (ensure(TargetFPS > 0.0f))
		{
			const float DyResFrameTimeBudget = 1000.0f / TargetFPS;
			CVarDyResFrameTimeBudget->Set(DyResFrameTimeBudget, ECVF_SetByGameSetting);
		}
	}
}

float USMSettingsLocal::GetOverallVolume() const
{
	return OverallVolume;
}

void USMSettingsLocal::SetOverallVolume(float InVolume)
{
	OverallVolume = InVolume;

	if (!bSoundControlBusMixLoaded)
	{
		LoadUserControlBusMix();
	}
	if (!bFMODLoaded)
	{
		LoadFMOD();
	}

	if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("Overall")))
	{
		if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
		{
			SetVolumeForControlBus(ControlBusPtr, OverallVolume);
		}
	}
	SetVolumeForVCA(FMOD_MasterVCA, OverallVolume);
}

float USMSettingsLocal::GetMusicVolume() const
{
	return MusicVolume;
}

void USMSettingsLocal::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;

	if (!bSoundControlBusMixLoaded)
	{
		LoadUserControlBusMix();
	}
	if (!bFMODLoaded)
	{
		LoadFMOD();
	}

	if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("Music")))
	{
		if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
		{
			SetVolumeForControlBus(ControlBusPtr, MusicVolume);
		}
	}
	SetVolumeForVCA(FMOD_MusicVCA, MusicVolume);
}

float USMSettingsLocal::GetSoundFXVolume() const
{
	return SoundFXVolume;
}

void USMSettingsLocal::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;

	if (!bSoundControlBusMixLoaded)
	{
		LoadUserControlBusMix();
	}
	if (!bFMODLoaded)
	{
		LoadFMOD();
	}

	if (TObjectPtr<USoundControlBus>* ControlBusDblPtr = ControlBusMap.Find(TEXT("SoundFX")))
	{
		if (USoundControlBus* ControlBusPtr = *ControlBusDblPtr)
		{
			SetVolumeForControlBus(ControlBusPtr, SoundFXVolume);
		}
	}
	SetVolumeForVCA(FMOD_SoundFxVCA, SoundFXVolume);
}

float USMSettingsLocal::GetVoiceChatVolume() const
{
	return VoiceChatVolume;
}

void USMSettingsLocal::SetVoiceChatVolume(float InVolume)
{
	VoiceChatVolume = InVolume;
	// TODO: VoiceChat 볼륨 설정
}

void USMSettingsLocal::SetVolumeForControlBus(USoundControlBus* InSoundControlBus, float InVolume)
{
	if (!bSoundControlBusMixLoaded)
	{
		LoadUserControlBusMix();
	}
	ensureMsgf(bSoundControlBusMixLoaded, TEXT("Failed to load UserControlBusMix Settings."));

	if (GEngine && InSoundControlBus && bSoundControlBusMixLoaded)
	{
		if (const UWorld* AudioWorld = GEngine->GetCurrentPlayWorld())
		{
			ensureMsgf(ControlBusMix, TEXT("Control Bus Mix failed to load."));

			FSoundControlBusMixStage UpdatedControlBusMixStage;
			UpdatedControlBusMixStage.Bus = InSoundControlBus;
			UpdatedControlBusMixStage.Value.TargetValue = InVolume;
			UpdatedControlBusMixStage.Value.AttackTime = 0.01f;
			UpdatedControlBusMixStage.Value.ReleaseTime = 0.01f;

			TArray<FSoundControlBusMixStage> UpdatedMixStageArray;
			UpdatedMixStageArray.Add(UpdatedControlBusMixStage);

			// UAudioModulationStatics::ActivateBusMix(this, ControlBusMix);
			UAudioModulationStatics::UpdateMix(AudioWorld, ControlBusMix, UpdatedMixStageArray);
		}
	}
}

void USMSettingsLocal::SetVolumeForVCA(UFMODVCA* TargetVCA, float InVolume)
{
	if (!bFMODLoaded)
	{
		LoadFMOD();
	}
	ensureMsgf(bFMODLoaded, TEXT("Failed to load FMOD assets."));

	if (TargetVCA)
	{
		UFMODBlueprintStatics::VCASetVolume(TargetVCA, InVolume);
	}
}

void USMSettingsLocal::LoadUserControlBusMix()
{
	if (GEngine)
	{
		if (const UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			if (const USMAudioSettings* AudioSettings = GetDefault<USMAudioSettings>())
			{
				USoundControlBus* OverallControlBus = nullptr;
				USoundControlBus* MusicControlBus = nullptr;
				USoundControlBus* SoundFXControlBus = nullptr;
				ControlBusMap.Empty();

				if (USoundControlBus* SoundControlBus = AudioSettings->GetOverallVolumeControlBus())
				{
					OverallControlBus = SoundControlBus;
					ControlBusMap.Add(TEXT("Overall"), OverallControlBus);
				}
				else
				{
					ensureMsgf(SoundControlBus, TEXT("Overall Control Bus reference missing from StereoMix Audio Settings."));
				}

				if (USoundControlBus* SoundControlBus = AudioSettings->GetMusicVolumeControlBus())
				{
					MusicControlBus = SoundControlBus;
					ControlBusMap.Add(TEXT("Music"), MusicControlBus);
				}
				else
				{
					ensureMsgf(SoundControlBus, TEXT("Music Control Bus reference missing from StereoMix Audio Settings."));
				}

				if (USoundControlBus* SoundControlBus = AudioSettings->GetSoundFXVolumeControlBus())
				{
					SoundFXControlBus = SoundControlBus;
					ControlBusMap.Add(TEXT("SoundFX"), SoundFXControlBus);
				}
				else
				{
					ensureMsgf(SoundControlBus, TEXT("SoundFX Control Bus reference missing from StereoMix Audio Settings."));
				}

				if (USoundControlBusMix* SoundControlBusMix = AudioSettings->GetUserSettingsControlBusMix())
				{
					ControlBusMix = SoundControlBusMix;

					const FSoundControlBusMixStage OverallControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, OverallControlBus, OverallVolume);
					const FSoundControlBusMixStage MusicControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, MusicControlBus, MusicVolume);
					const FSoundControlBusMixStage SoundFXControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, SoundFXControlBus, SoundFXVolume);

					TArray<FSoundControlBusMixStage> ControlBusMixStages;
					ControlBusMixStages.Add(OverallControlBusMixStage);
					ControlBusMixStages.Add(MusicControlBusMixStage);
					ControlBusMixStages.Add(SoundFXControlBusMixStage);

					UAudioModulationStatics::UpdateMix(World, ControlBusMix, ControlBusMixStages);

					bSoundControlBusMixLoaded = true;
				}
				else
				{
					ensureMsgf(SoundControlBusMix, TEXT("User Settings Control Bus Mix reference missing from StereoMix Audio Settings."));
				}
			}
		}
	}
}

void USMSettingsLocal::LoadFMOD()
{
	if (GEngine)
	{
		if (const USMAudioSettings* AudioSettings = GetDefault<USMAudioSettings>())
		{
			if (UFMODVCA* VCA = AudioSettings->GetMasterVCA())
			{
				FMOD_MasterVCA = VCA;
			}
			else
			{
				ensureMsgf(VCA, TEXT("Master VCA reference missing from StereoMix AudioSettings."));
			}

			if (UFMODVCA* VCA = AudioSettings->GetMusicVCA())
			{
				FMOD_MusicVCA = VCA;
			}
			else
			{
				ensureMsgf(VCA, TEXT("Music VCA reference missing from StereoMix AudioSettings."));
			}

			if (UFMODVCA* VCA = AudioSettings->GetSoundFxVCA())
			{
				FMOD_SoundFxVCA = VCA;
			}
			else
			{
				ensureMsgf(VCA, TEXT("SoundFX VCA reference missing from StereoMix AudioSettings."));
			}

			bFMODLoaded = true;
		}
	}
}

void USMSettingsLocal::SetAudioOutputDeviceId(const FString& InAudioOutputDeviceId)
{
	AudioOutputDeviceId = InAudioOutputDeviceId;
	OnAudioDeviceChangedEvent.Broadcast(InAudioOutputDeviceId);
}

void USMSettingsLocal::SetControllerPlatform(const FName InControllerPlatform)
{
	if (ControllerPlatform != InControllerPlatform)
	{
		ControllerPlatform = InControllerPlatform;
		if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetTypedOuter<ULocalPlayer>()))
		{
			InputSubsystem->SetGamepadInputType(ControllerPlatform);
		}
	}
}

FName USMSettingsLocal::GetControllerPlatform() const
{
	return ControllerPlatform;
}
