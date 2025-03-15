// Copyright Studio Surround. All Rights Reserved.

#include "GameSettingAction.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "NativeGameplayTags.h"
#include "SMGameSettingRegistry.h"
#include "SMSettingsLocal.h"
#include "SMSettingsShared.h"
#include "CustomSettings/SMSettingValueDiscrete_OverallQuality.h"
#include "CustomSettings/SMSettingValueDiscrete_Resolution.h"
#include "DataSource/GameSettingDataSourceDynamic.h"
#include "EditCondition/WhenCondition.h"
#include "EditCondition/WhenPlatformHasTrait.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Internationalization/StringTableRegistry.h"
#include "Performance/SMPerformanceSettings.h"

#define LOCTABLE_ID "Settings"

UE_DEFINE_GAMEPLAY_TAG_STATIC(GameSettings_Action_EditSafeZone, "GameSettings.Action.EditSafeZone");
UE_DEFINE_GAMEPLAY_TAG_STATIC(GameSettings_Action_EditBrightness, "GameSettings.Action.EditBrightness");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsWindowedMode, "Platform.Trait.SupportsWindowedMode");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_NeedsBrightnessAdjustment, "Platform.Trait.NeedsBrightnessAdjustment");

enum class EFramePacingEditCondition
{
	EnableIf,
	DisableIf
};

class FGameSettingEditCondition_FramePacingMode : public FGameSettingEditCondition
{
public:
	FGameSettingEditCondition_FramePacingMode(ESMFramePacingMode InDesiredMode, EFramePacingEditCondition InMatchMode = EFramePacingEditCondition::EnableIf)
		: DesiredMode(InDesiredMode), MatchMode(InMatchMode)
	{
	}

	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		const ESMFramePacingMode ActualMode = USMPlatformSpecificRenderingSettings::Get().FramePacingMode;
		const bool bMatches = ActualMode == DesiredMode;
		const bool bMatchesAreBad = MatchMode == EFramePacingEditCondition::DisableIf;
		if (bMatches == bMatchesAreBad)
		{
			InOutEditState.Kill(FString::Printf(TEXT("Frame pacing mode %d didn't match requirement %d"), static_cast<int32>(ActualMode), static_cast<int32>(DesiredMode)));
		}
	}

private:
	ESMFramePacingMode DesiredMode;
	EFramePacingEditCondition MatchMode;
};

class FGameSettingEditCondition_VideoQuality : public FGameSettingEditCondition
{
public:
	FGameSettingEditCondition_VideoQuality(const FString& InDisableString)
		: DisableString(InDisableString)
	{
	}

	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		if (!USMPlatformSpecificRenderingSettings::Get().bSupportsAutomaticVideoQualityBenchmark)
		{
			InOutEditState.Kill(DisableString);
		}
	}

	virtual void SettingChanged(const ULocalPlayer* InLocalPlayer, UGameSetting* Setting, EGameSettingChangeReason Reason) const override
	{
		const USMLocalPlayer* LocalPlayer = CastChecked<USMLocalPlayer>(InLocalPlayer);
		LocalPlayer->GetLocalSettings()->ApplyScalabilitySettings();
	}

private:
	FString DisableString;
};

UGameSettingCollection* USMGameSettingRegistry::InitializeVideoSettings(USMLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("VideoCollection"));
	Screen->SetDisplayName(LOCTABLE(LOCTABLE_ID, "VideoCollection"));
	Screen->Initialize(InLocalPlayer);

	UGameSettingValueDiscreteDynamic_Enum* WindowModeSetting;

	//////////////////////////////////////////////////////////////////
	// Display
	{
		UGameSettingCollection* Display = NewObject<UGameSettingCollection>();
		Display->SetDevName(TEXT("DisplayCollection"));
		Display->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_DisplayCollection"));
		Screen->AddSetting(Display);

		//////////////////////////////////////////////////////////////////
		// WindowMode
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("WindowMode"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_WindowMode"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_WindowMode_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetFullscreenMode));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetFullscreenMode));
			Setting->AddEnumOption(EWindowMode::Fullscreen, LOCTABLE(LOCTABLE_ID, "Video_WindowMode_Fullscreen"));
			Setting->AddEnumOption(EWindowMode::WindowedFullscreen, LOCTABLE(LOCTABLE_ID, "Video_WindowMode_WindowedFullscreen"));
			Setting->AddEnumOption(EWindowMode::Windowed, LOCTABLE(LOCTABLE_ID, "Video_WindowMode_Windowed"));

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_SupportsWindowedMode, TEXT("Platform does not support window mode")));

			WindowModeSetting = Setting;

			Display->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Resolution
		{
			USMSettingValueDiscrete_Resolution* Setting = NewObject<USMSettingValueDiscrete_Resolution>();
			Setting->SetDevName(TEXT("Resolution"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_Resolution"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_Resolution_Desc"));

			Setting->AddEditDependency(WindowModeSetting);
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_SupportsWindowedMode, TEXT("Platform does not support window mode")));
			Setting->AddEditCondition(MakeShared<FWhenCondition>([WindowModeSetting](const ULocalPlayer*, FGameSettingEditableState& InOutEditState) {
				if (WindowModeSetting->GetValue<EWindowMode::Type>() == EWindowMode::WindowedFullscreen)
				{
					InOutEditState.Disable(LOCTABLE(LOCTABLE_ID, "Video_Resolution_WindowedFullscreen_Disabled"));
				}
			}));

			Display->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Performance
		{
			AddPerformanceStatPage(Display, InLocalPlayer);
		}
	}
	//////////////////////////////////////////////////////////////////
	// Graphics
	{
		UGameSettingCollection* Graphics = NewObject<UGameSettingCollection>();
		Graphics->SetDevName(TEXT("GraphicsCollection"));
		Graphics->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_GraphicsCollection"));
		Screen->AddSetting(Graphics);

		//////////////////////////////////////////////////////////////////
		// ColorBlindMode
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("ColorBlindMode"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_ColorBlindMode"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_ColorBlindMode_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetColorBlindMode));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetColorBlindMode));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetColorBlindMode());
			Setting->AddEnumOption(EColorBlindMode::Off, LOCTABLE(LOCTABLE_ID, "Video_ColorBlindMode_Off"));
			Setting->AddEnumOption(EColorBlindMode::Deuteranope, LOCTABLE(LOCTABLE_ID, "Video_ColorBlindMode_Deuteranope"));
			Setting->AddEnumOption(EColorBlindMode::Protanope, LOCTABLE(LOCTABLE_ID, "Video_ColorBlindMode_Protanope"));
			Setting->AddEnumOption(EColorBlindMode::Tritanope, LOCTABLE(LOCTABLE_ID, "Video_ColorBlindMode_Tritanope"));

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			Graphics->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// ColorBlindStrength
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("ColorBlindStrength"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_ColorBlindStrength"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_ColorBlindStrength_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetColorBlindStrength));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetColorBlindStrength));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetColorBlindStrength());
			for (int32 Index = 0; Index <= 10; Index++)
			{
				Setting->AddOption(Index, FText::AsNumber(Index));
			}

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			Graphics->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Brightness
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("Brightness"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_Brightness"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_Brightness_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetDisplayGamma));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetDisplayGamma));
			Setting->SetDefaultValue(2.2);
			Setting->SetDisplayFormat([](double SourceValue, double NormalizedValue) {
				return FText::Format(LOCTABLE(LOCTABLE_ID, "Video_Brightness_Format"), static_cast<int32>(FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(50, 150), NormalizedValue)));
			});
			Setting->SetSourceRangeAndStep(TRange<double>(1.7, 2.7), 0.01);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_NeedsBrightnessAdjustment, TEXT("Platform does not require brightness adjustment.")));

			Graphics->AddSetting(Setting);
		}
	}
	//////////////////////////////////////////////////////////////////
	// Graphics Quality
	{
		UGameSettingCollection* GraphicsQuality = NewObject<UGameSettingCollection>();
		GraphicsQuality->SetDevName(TEXT("GraphicsQuality"));
		GraphicsQuality->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_GraphicsQuality"));
		Screen->AddSetting(GraphicsQuality);

		UGameSetting* AutoSetQuality;
		UGameSetting* GraphicsQualityPresets;

		//////////////////////////////////////////////////////////////////
		// Device Profile Suffix
		{
			// Console-style device profile selection
			UGameSettingValueDiscreteDynamic* Setting = NewObject<UGameSettingValueDiscreteDynamic>();
			Setting->SetDevName(TEXT("DeviceProfileSuffix"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_DeviceProfileSuffix"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_DeviceProfileSuffix_Desc"));
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetDesiredDeviceProfileQualitySuffix));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetDesiredDeviceProfileQualitySuffix));

			const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();

			Setting->SetDefaultValueFromString(PlatformSettings.DefaultDeviceProfileSuffix);
			for (const auto& [DisplayName, DeviceProfileSuffix, MinRefreshRate] : PlatformSettings.UserFacingDeviceProfileOptions)
			{
				if (FPlatformMisc::GetMaxRefreshRate() >= MinRefreshRate)
				{
					Setting->AddDynamicOption(DeviceProfileSuffix, DisplayName);
				}
			}

			if (Setting->GetDynamicOptions().Num() > 1)
			{
				GraphicsQuality->AddSetting(Setting);
			}
		}
		//////////////////////////////////////////////////////////////////
		// AutoSetQuality
		{
			UGameSettingAction* Setting = NewObject<UGameSettingAction>();
			Setting->SetDevName(TEXT("AutoSetQuality"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_AutoSetQuality"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_AutoSetQuality_Desc"));

			Setting->SetDoesActionDirtySettings(true);
			Setting->SetActionText(LOCTABLE(LOCTABLE_ID, "Video_AutoSetQuality_Action"));
			Setting->SetCustomAction([](ULocalPlayer* LocalPlayer) {
				const USMLocalPlayer* StereoMixLocalPlayer = CastChecked<USMLocalPlayer>(LocalPlayer);
				// We don't save state until users apply the settings.
				constexpr bool bImmediatelySaveState = false;
				StereoMixLocalPlayer->GetLocalSettings()->RunAutoBenchmark(bImmediatelySaveState);
			});

			Setting->AddEditCondition(MakeShared<FWhenCondition>([](const ULocalPlayer* LocalPlayer, FGameSettingEditableState& InOutEditState) {
				const USMPlatformSpecificRenderingSettings& PlatformSettings = USMPlatformSpecificRenderingSettings::Get();
				const bool bCanUseDueToMobile = (PlatformSettings.FramePacingMode == ESMFramePacingMode::MobileStyle);

				const USMLocalPlayer* StereoMixLocalPlayer = CastChecked<USMLocalPlayer>(LocalPlayer);
				const bool bCanBenchmark = StereoMixLocalPlayer->GetLocalSettings()->CanRunAutoBenchmark();

				if (!bCanUseDueToMobile && !bCanBenchmark)
				{
					InOutEditState.Kill(TEXT("Auto quality not supported"));
				}
			}));

			GraphicsQuality->AddSetting(Setting);
			AutoSetQuality = Setting;
		}
		//////////////////////////////////////////////////////////////////
		// GraphicsQualityPresets
		{
			USMSettingValueDiscrete_OverallQuality* Setting = NewObject<USMSettingValueDiscrete_OverallQuality>();
			Setting->SetDevName(TEXT("GraphicsQualityPresets"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_GraphicsQualityPresets"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_GraphicsQualityPresets_Desc"));
			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(ESMFramePacingMode::ConsoleStyle, EFramePacingEditCondition::DisableIf));

			GraphicsQuality->AddSetting(Setting);
			GraphicsQualityPresets = Setting;
		}
		//////////////////////////////////////////////////////////////////
		// ResolutionScale
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("ResolutionScale"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_ResolutionScale"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_ResolutionScale_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetResolutionScaleNormalized));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetResolutionScaleNormalized));
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support 3D Resolution")));
			//@TODO: Add support for 3d res on mobile

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);
			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// GlobalIlluminationQuality
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("GlobalIlluminationQuality"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_GlobalIlluminationQuality"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_GlobalIlluminationQuality_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetGlobalIlluminationQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetGlobalIlluminationQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support GlobalIlluminationQuality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Shadows
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("Shadows"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_Shadows"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_Shadows_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetShadowQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetShadowQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support Shadows")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Anti Aliasing
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("AntiAliasing"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_AntiAliasing"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_AntiAliasing_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetAntiAliasingQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetAntiAliasingQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support Anti-Aliasing")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// ViewDistance
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("ViewDistance"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_ViewDistance"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_ViewDistance_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetViewDistanceQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetViewDistanceQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support View Distance")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Texture Quality
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("TextureQuality"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_TextureQuality"));

			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_TextureQuality_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetTextureQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetTextureQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support Texture quality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// VisualEffectQuality
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("VisualEffectQuality"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_VisualEffectQuality"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_VisualEffectQuality_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetVisualEffectQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetVisualEffectQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support VisualEffectQuality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// ReflectionQuality
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("ReflectionQuality"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_ReflectionQuality"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_ReflectionQuality_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetReflectionQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetReflectionQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support ReflectionQuality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// PostProcess Quality
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("PostProcessingQuality"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_PostProcessingQuality"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_PostProcessingQuality_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetPostProcessingQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetPostProcessingQuality));
			Setting->AddOption(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
			Setting->AddOption(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
			Setting->AddOption(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
			Setting->AddOption(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support PostProcessingQuality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
	}
	//////////////////////////////////////////////////////////////////
	// Advanced Graphics
	{
		UGameSettingCollection* AdvancedGraphics = NewObject<UGameSettingCollection>();
		AdvancedGraphics->SetDevName(TEXT("AdvancedGraphics"));
		AdvancedGraphics->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_AdvancedGraphics"));
		Screen->AddSetting(AdvancedGraphics);

		//////////////////////////////////////////////////////////////////
		// Vertical Sync
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("VerticalSync"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_VerticalSync"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_VerticalSync_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(IsVSyncEnabled));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetVSyncEnabled));
			Setting->SetDefaultValue(false);

			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(ESMFramePacingMode::DesktopStyle));

			Setting->AddEditDependency(WindowModeSetting);
			Setting->AddEditCondition(MakeShared<FWhenCondition>([WindowModeSetting](const ULocalPlayer*, FGameSettingEditableState& InOutEditState) {
				if (WindowModeSetting->GetValue<EWindowMode::Type>() != EWindowMode::Fullscreen)
				{
					InOutEditState.Disable(LOCTABLE(LOCTABLE_ID, "Video_VerticalSync_FullscreenNeededForVSync"));
				}
			}));

			AdvancedGraphics->AddSetting(Setting);
		}
	}

	return Screen;
}

void AddFrameRateOptions(UGameSettingValueDiscreteDynamic_Number* Setting)
{
	const FText FPSFormat = LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_Format");
	for (const int32 Rate : GetDefault<USMPerformanceSettings>()->DesktopFrameRateLimits)
	{
		Setting->AddOption(static_cast<float>(Rate), FText::Format(FPSFormat, Rate));
	}
	Setting->AddOption(0.0f, LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_Unlimited"));
}

void USMGameSettingRegistry::InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, USMLocalPlayer* InLocalPlayer)
{
	//////////////////////////////////////////////////////////////////
	// FrameRateLimit - Always
	{
		UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
		Setting->SetDevName(TEXT("FrameRateLimit_Always"));
		Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_Always"));
		Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_Always_Desc"));

		Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetFrameRateLimit_Always));
		Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetFrameRateLimit_Always));
		Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetFrameRateLimit_Always());
		Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(ESMFramePacingMode::DesktopStyle));

		AddFrameRateOptions(Setting);

		Screen->AddSetting(Setting);
	}
	//////////////////////////////////////////////////////////////////
	// FrameRateLimit - InMenu
	{
		UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
		Setting->SetDevName(TEXT("FrameRateLimit_InMenu"));
		Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_InMenu"));
		Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_InMenu_Desc"));

		Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetFrameRateLimit_InMenu));
		Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetFrameRateLimit_InMenu));
		Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetFrameRateLimit_InMenu());
		Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(ESMFramePacingMode::DesktopStyle));

		AddFrameRateOptions(Setting);

		Screen->AddSetting(Setting);
	}
	//////////////////////////////////////////////////////////////////
	// FrameRateLimit - WhenBackground
	{
		UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
		Setting->SetDevName(TEXT("FrameRateLimit_Background"));
		Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_Background"));
		Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Video_FrameRateLimit_Background_Desc"));

		Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetFrameRateLimit_WhenBackground));
		Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetFrameRateLimit_WhenBackground));
		Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetFrameRateLimit_WhenBackground());
		Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(ESMFramePacingMode::DesktopStyle));

		AddFrameRateOptions(Setting);

		Screen->AddSetting(Setting);
	}
}

#undef LOCTABLE_ID
