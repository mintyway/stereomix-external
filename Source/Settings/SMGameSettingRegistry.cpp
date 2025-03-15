// Copyright Studio Surround. All Rights Reserved.


#include "SMGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "SMSettingsLocal.h"
#include "SMSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameSettingRegistry)

#define LOCTABLE_ID "Settings"

static const TCHAR* SettingRegistryName = TEXT("StereoMixGameSettingRegistry");

USMGameSettingRegistry::USMGameSettingRegistry()
{
}

USMGameSettingRegistry* USMGameSettingRegistry::Get(USMLocalPlayer* InLocalPlayer)
{
	USMGameSettingRegistry* Registry = FindObject<USMGameSettingRegistry>(InLocalPlayer, SettingRegistryName, true);
	if (!Registry)
	{
		Registry = NewObject<USMGameSettingRegistry>(InLocalPlayer, SettingRegistryName);
		Registry->Initialize(InLocalPlayer);
	}
	return Registry;
}

void USMGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	if (USMLocalPlayer* LocalPlayer = Cast<USMLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

void USMGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	USMLocalPlayer* LocalPlayer = CastChecked<USMLocalPlayer>(InLocalPlayer);

	GameplaySettings = InitializeGameplaySettings(LocalPlayer);
	RegisterSetting(GameplaySettings);

	VideoSettings = InitializeVideoSettings(LocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, LocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(LocalPlayer);
	RegisterSetting(AudioSettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(LocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(LocalPlayer);
	RegisterSetting(GamepadSettings);
}

bool USMGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (const USMLocalPlayer* LocalPlayer = CastChecked<USMLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() != nullptr)
			{
				return true;
			}
		}
	}
	return false;
}

#undef LOCTABLE_ID
