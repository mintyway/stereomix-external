// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingsShared.h"

#include "EnhancedInputSubsystems.h"
#include "Internationalization/Culture.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingsShared)

static FString SharedSettingsSlotName = TEXT("SharedGameSettings");

namespace StereoMix::SettingsSharedCVars
{
	static float DefaultGamepadLeftStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadLeftStickInnerDeadZone(
		TEXT("gpad.DefaultLeftStickInnerDeadZone"),
		DefaultGamepadLeftStickInnerDeadZone,
		TEXT("Gamepad left stick inner dead zone"));

	static float DefaultGamepadRightStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadRightStickInnerDeadZone(
		TEXT("gpad.DefaultRightStickInnerDeadZone"),
		DefaultGamepadRightStickInnerDeadZone,
		TEXT("Gamepad right stick inner dead zone"));
}

USMSettingsShared::USMSettingsShared()
{
	FInternationalization::Get().OnCultureChanged().AddUObject(this, &ThisClass::OnCultureChanged);

	GamepadMoveStickDeadZone = StereoMix::SettingsSharedCVars::DefaultGamepadLeftStickInnerDeadZone;
	GamepadLookStickDeadZone = StereoMix::SettingsSharedCVars::DefaultGamepadRightStickInnerDeadZone;
}

int32 USMSettingsShared::GetLatestDataVersion() const
{
	return 1;
}

USMSettingsShared* USMSettingsShared::CreateTemporarySettings(const USMLocalPlayer* LocalPlayer)
{
	USMSettingsShared* SharedSettings = Cast<USMSettingsShared>(CreateNewSaveGameForLocalPlayer(StaticClass(), LocalPlayer, SharedSettingsSlotName));
	SharedSettings->ApplySettings();
	return SharedSettings;
}

USMSettingsShared* USMSettingsShared::LoadOrCreateSettings(const USMLocalPlayer* LocalPlayer)
{
	USMSettingsShared* SharedSettings = Cast<USMSettingsShared>(LoadOrCreateSaveGameForLocalPlayer(StaticClass(), LocalPlayer, SharedSettingsSlotName));
	SharedSettings->ApplySettings();
	return SharedSettings;
}

bool USMSettingsShared::AsyncLoadOrCreateSettings(const USMLocalPlayer* LocalPlayer, FSMOnSettingsLoaded OnSettingsLoaded)
{
	FOnLocalPlayerSaveGameLoadedNative Lambda = FOnLocalPlayerSaveGameLoadedNative::CreateLambda([OnSettingsLoaded](ULocalPlayerSaveGame* LoadedSave) {
		USMSettingsShared* LoadedSettings = CastChecked<USMSettingsShared>(LoadedSave);
		LoadedSettings->ApplySettings();
		OnSettingsLoaded.ExecuteIfBound(LoadedSettings);
	});
	return AsyncLoadOrCreateSaveGameForLocalPlayer(StaticClass(), LocalPlayer, SharedSettingsSlotName, Lambda);
}

void USMSettingsShared::SaveSettings()
{
	AsyncSaveGameToSlotForLocalPlayer();
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->AsyncSaveSettings();
		}
	}
}

void USMSettingsShared::ApplySettings()
{
	ApplyBackgroundAudioSettings();
	ApplyCultureSettings();
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->ApplySettings();
		}
	}
}

EColorBlindMode USMSettingsShared::GetColorBlindMode() const
{
	return ColorBlindMode;
}

void USMSettingsShared::SetColorBlindMode(EColorBlindMode InMode)
{
	if (ColorBlindMode != InMode)
	{
		ColorBlindMode = InMode;
		ApplyColorBlindSettings();
	}
}

int32 USMSettingsShared::GetColorBlindStrength() const
{
	return ColorBlindStrength;
}

void USMSettingsShared::SetColorBlindStrength(int32 InColorBlindStrength)
{
	InColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = InColorBlindStrength;
		ApplyColorBlindSettings();
	}
}

void USMSettingsShared::ApplyColorBlindSettings()
{
	FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
		static_cast<EColorVisionDeficiency>(static_cast<int32>(ColorBlindMode)),
		ColorBlindStrength,
		true,
		false);
}

void USMSettingsShared::SetAllowAudioInBackgroundSetting(ESMAllowBackgroundAudioSetting NewValue)
{
	if (ChangeValueAndDirty(AllowAudioInBackground, NewValue))
	{
		ApplyBackgroundAudioSettings();
	}
}

void USMSettingsShared::ApplyBackgroundAudioSettings()
{
	if (OwningPlayer && OwningPlayer->IsPrimaryPlayer())
	{
		FApp::SetUnfocusedVolumeMultiplier(AllowAudioInBackground != ESMAllowBackgroundAudioSetting::Off ? 1.0f : 0.0f);
	}
}

const FString& USMSettingsShared::GetPendingCulture() const
{
	return PendingCulture;
}

void USMSettingsShared::SetPendingCulture(const FString& NewCulture)
{
	PendingCulture = NewCulture;
	bResetToDefaultCulture = false;
	bIsDirty = true;
}

void USMSettingsShared::SetAndApplyCulture(const FString& NewCulture)
{
	SetPendingCulture(NewCulture);
	ApplyCultureSettings();
}

void USMSettingsShared::OnCultureChanged()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void USMSettingsShared::ClearPendingCulture()
{
	PendingCulture.Reset();
}

bool USMSettingsShared::IsUsingDefaultCulture() const
{
	FString Culture;
	GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);
	return Culture.IsEmpty();
}

void USMSettingsShared::ResetToDefaultCulture()
{
	ClearPendingCulture();
	bResetToDefaultCulture = true;
	bIsDirty = true;
}

void USMSettingsShared::ApplyCultureSettings()
{
	if (bResetToDefaultCulture)
	{
		const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
		check(SystemDefaultCulture.IsValid());

		const FString CultureToApply = SystemDefaultCulture->GetName();
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		bResetToDefaultCulture = false;
	}
	else if (!PendingCulture.IsEmpty())
	{
		const FString CultureToApply = PendingCulture;
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		ClearPendingCulture();
	}
}

void USMSettingsShared::ResetCultureToCurrentSettings()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void USMSettingsShared::ApplyInputSensitivity()
{
	// TODO: Implement
}
