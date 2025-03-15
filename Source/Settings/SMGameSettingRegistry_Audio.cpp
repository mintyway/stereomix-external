// Copyright Studio Surround. All Rights Reserved.

#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "SMGameSettingRegistry.h"
#include "NativeGameplayTags.h"
#include "SMSettingsLocal.h"
#include "SMSettingsShared.h"
#include "CustomSettings/SMSettingValueDiscreteDynamic_AudioOutputDevice.h"
#include "DataSource/GameSettingDataSourceDynamic.h"
#include "EditCondition/WhenPlatformHasTrait.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Internationalization/StringTableRegistry.h"

#define LOCTABLE_ID "Settings"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsChangingAudioOutputDevice, "Platform.Trait.SupportsChangingAudioOutputDevice");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsBackgroundAudio, "Platform.Trait.SupportsBackgroundAudio");

UGameSettingCollection* USMGameSettingRegistry::InitializeAudioSettings(USMLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("AudioCollection"));
	Screen->SetDisplayName(LOCTABLE(LOCTABLE_ID, "AudioCollection"));
	Screen->Initialize(InLocalPlayer);

	//////////////////////////////////////////////////////////////////
	// Volume
	{
		UGameSettingCollection* Volume = NewObject<UGameSettingCollection>();
		Volume->SetDevName(TEXT("VolumeCollection"));
		Volume->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_VolumeCollection"));
		Screen->AddSetting(Volume);

		//////////////////////////////////////////////////////////////////
		// Overall Volume
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("OverallVolume"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_OverallVolume"));;
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Audio_OverallVolume_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetOverallVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetOverallVolume));
			Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetOverallVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			Volume->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Music Volume
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MusicVolume"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_MusicVolume"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Audio_MusicVolume_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMusicVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMusicVolume));
			Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetMusicVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			Volume->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Sound Effects Volume
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("SoundEffectsVolume"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_SoundEffectsVolume"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Audio_SoundEffectsVolume_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSoundFXVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSoundFXVolume));
			Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetSoundFXVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			Volume->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Voice Chat Volume
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("VoiceChatVolume"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_VoiceChatVolume"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Audio_VoiceChatVolume_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetVoiceChatVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetVoiceChatVolume));
			Setting->SetDefaultValue(GetDefault<USMSettingsLocal>()->GetVoiceChatVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			Volume->AddSetting(Setting);
		}
	}


	//////////////////////////////////////////////////////////////////
	// Sound
	{
		UGameSettingCollection* Sound = NewObject<UGameSettingCollection>();
		Sound->SetDevName(TEXT("SoundCollection"));
		Sound->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_SoundCollection"));
		Screen->AddSetting(Sound);

		//////////////////////////////////////////////////////////////////
		// Audio Output Device
		{
			USMSettingValueDiscreteDynamic_AudioOutputDevice* Setting = NewObject<USMSettingValueDiscreteDynamic_AudioOutputDevice>();
			Setting->SetDevName(TEXT("AudioOutputDevice"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_AudioOutputDevice"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Audio_AudioOutputDevice_Desc"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetAudioOutputDeviceId));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetAudioOutputDeviceId));

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(
					TAG_Platform_Trait_SupportsChangingAudioOutputDevice,
					TEXT("Platform does not support changing audio output device"))
				);

			Sound->AddSetting(Setting);
		}
		//////////////////////////////////////////////////////////////////
		// Background Audio
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("BackgroundAudio"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Audio_BackgroundAudio"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Audio_BackgroundAudio_Desc"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetAllowAudioInBackgroundSetting));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetAllowAudioInBackgroundSetting));
			Setting->SetDefaultValue(GetDefault<USMSettingsShared>()->GetAllowAudioInBackgroundSetting());

			Setting->AddEnumOption(ESMAllowBackgroundAudioSetting::Off, LOCTABLE(LOCTABLE_ID, "Options_Off"));
			Setting->AddEnumOption(ESMAllowBackgroundAudioSetting::AllSounds, LOCTABLE(LOCTABLE_ID, "Options_On"));

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(
					TAG_Platform_Trait_SupportsBackgroundAudio,
					TEXT("Platform does not support background audio"))
				);

			Sound->AddSetting(Setting);
		}
	}

	return Screen;
}

#undef LOCTABLE_ID
