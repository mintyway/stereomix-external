// Copyright Studio Surround. All Rights Reserved.

#include "CommonInputBaseTypes.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "SMGameSettingRegistry.h"
#include "NativeGameplayTags.h"
#include "SMSettingsLocal.h"
#include "SMSettingsShared.h"
#include "DataSource/GameSettingDataSourceDynamic.h"
#include "EditCondition/WhenPlatformHasTrait.h"
#include "Internationalization/StringTableRegistry.h"

#define LOCTABLE_ID "Settings"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_SupportsGamepad, "Platform.Trait.Input.SupportsGamepad");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_SupportsTriggerHaptics, "Platform.Trait.Input.SupportsTriggerHaptics");

UGameSettingCollection* USMGameSettingRegistry::InitializeGamepadSettings(USMLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("GamepadCollection"));
	Screen->SetDisplayName(LOCTABLE(LOCTABLE_ID, "GamepadCollection"));

	Screen->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));
	Screen->Initialize(InLocalPlayer);

	//////////////////////////////////////////////////////////////////
	// Hardware
	{
		UGameSettingCollection* Hardware = NewObject<UGameSettingCollection>();
		Hardware->SetDevName(TEXT("HardwareCollection"));
		Hardware->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_HardwareCollection"));
		Hardware->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));
		Screen->AddSetting(Hardware);

		//////////////////////////////////////////////////////////////////
		// Controller
		{
			UGameSettingValueDiscreteDynamic* Setting = NewObject<UGameSettingValueDiscreteDynamic>();
			Setting->SetDevName(TEXT("ControllerHardware"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_ControllerHardware"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_ControllerHardware_Desc"));
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetControllerPlatform));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetControllerPlatform));

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			if (UCommonInputPlatformSettings* PlatformInputSettings = UPlatformSettingsManager::Get().GetSettingsForPlatform<UCommonInputPlatformSettings>())
			{
				const TArray<TSoftClassPtr<UCommonInputBaseControllerData>>& ControllerDatas = PlatformInputSettings->GetControllerData();
				for (TSoftClassPtr ControllerDataPtr : ControllerDatas)
				{
					if (TSubclassOf<UCommonInputBaseControllerData> ControllerDataClass = ControllerDataPtr.LoadSynchronous())
					{
						const UCommonInputBaseControllerData* ControllerData = ControllerDataClass.GetDefaultObject();
						if (ControllerData->InputType == ECommonInputType::Gamepad)
						{
							Setting->AddDynamicOption(ControllerData->GamepadName.ToString(), ControllerData->GamepadDisplayName);
						}
					}
				}

				// Add the setting if we can select more than one game controller type on this platform
				// and we are allowed to change it
				if (Setting->GetDynamicOptions().Num() > 1 && PlatformInputSettings->CanChangeGamepadType())
				{
					Hardware->AddSetting(Setting);

					const FName CurrentControllerPlatform = GetDefault<USMSettingsLocal>()->GetControllerPlatform();
					if (CurrentControllerPlatform == NAME_None)
					{
						Setting->SetDiscreteOptionByIndex(0);
					}
					else
					{
						Setting->SetDefaultValueFromString(CurrentControllerPlatform.ToString());
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////
		// Gamepad Vibration
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GamepadVibration"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadVibration"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadVibration_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetForceFeedbackEnabled));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetForceFeedbackEnabled));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetForceFeedbackEnabled());

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			Hardware->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Invert Vertical Axis
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertVerticalAxis_Gamepad"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadInvertVerticalAxis"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadInvertVerticalAxis_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertVerticalAxis));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertVerticalAxis));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetInvertVerticalAxis());

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			Hardware->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Invert Horizontal Axis
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertHorizontalAxis_Gamepad"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadInvertHorizontalAxis"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadInvertHorizontalAxis_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertHorizontalAxis));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertHorizontalAxis));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetInvertHorizontalAxis());

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			Hardware->AddSetting(Setting);
		}
	}

	//////////////////////////////////////////////////////////////////
	// Gamepad Binding
	{
		UGameSettingCollection* GamepadBinding = NewObject<UGameSettingCollection>();
		GamepadBinding->SetDevName(TEXT("GamepadBindingCollection"));
		GamepadBinding->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadBindingCollection"));

		GamepadBinding->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

		Screen->AddSetting(GamepadBinding);
	}
	//////////////////////////////////////////////////////////////////
	// Basic - Look Sensitivity
	{
		UGameSettingCollection* BasicSensitivity = NewObject<UGameSettingCollection>();
		BasicSensitivity->SetDevName(TEXT("BasicSensitivityCollection"));
		BasicSensitivity->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_BasicSensitivityCollection"));

		BasicSensitivity->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

		Screen->AddSetting(BasicSensitivity);

		const FText EGamepadSensitivityText[] = {
			FText::GetEmpty(),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_Slow"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_SlowPlus"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_SlowPlusPlus"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_Normal"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_NormalPlus"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_NormalPlusPlus"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_Fast"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_FastPlus"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_FastPlusPlus"),
			LOCTABLE(LOCTABLE_ID, "Gamepad_GamepadSensitivity_Insane"),
		};

		//////////////////////////////////////////////////////////////////
		// Look Sensitivity
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("LookSensitivityPreset"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_LookSensitivityPreset"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_LookSensitivityPreset_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetGamepadLookSensitivityPreset));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetLookSensitivityPreset));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetGamepadLookSensitivityPreset());

			for (int32 PresetIndex = 1; PresetIndex < static_cast<int32>(ESMGamepadSensitivity::Num); PresetIndex++)
			{
				Setting->AddEnumOption(static_cast<ESMGamepadSensitivity>(PresetIndex), EGamepadSensitivityText[PresetIndex]);
			}

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			BasicSensitivity->AddSetting(Setting);
		}
	}

	//////////////////////////////////////////////////////////////////
	// Dead Zone
	{
		UGameSettingCollection* DeadZone = NewObject<UGameSettingCollection>();
		DeadZone->SetDevName(TEXT("DeadZoneCollection"));
		DeadZone->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_DeadZoneCollection"));
		Screen->AddSetting(DeadZone);

		//////////////////////////////////////////////////////////////////
		// Left Stick DeadZone
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MoveStickDeadZone"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_MoveStickDeadZone"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_MoveStickDeadZone_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetGamepadMoveStickDeadZone));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetGamepadMoveStickDeadZone));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetGamepadMoveStickDeadZone());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
			Setting->SetMinimumLimit(0.05);
			Setting->SetMaximumLimit(0.95);

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			DeadZone->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Right Stick DeadZone
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("LookStickDeadZone"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gamepad_LookStickDeadZone"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gamepad_LookStickDeadZone_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetGamepadLookStickDeadZone));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetGamepadLookStickDeadZone));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetGamepadLookStickDeadZone());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
			Setting->SetMinimumLimit(0.05);
			Setting->SetMaximumLimit(0.95);

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_Input_SupportsGamepad, TEXT("Platform does not support gamepad input")));

			DeadZone->AddSetting(Setting);
		}
	}

	return Screen;
}

#undef LOCTABLE_ID
