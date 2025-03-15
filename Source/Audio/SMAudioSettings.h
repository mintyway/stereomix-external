// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FMODVCA.h"
#include "FMODBus.h"
#include "SoundControlBus.h"
#include "SoundControlBusMix.h"
#include "SMAudioSettings.generated.h"


UCLASS(Config=Game, DefaultConfig, MinimalAPI, DisplayName="StereoMix Audio Settings")
class USMAudioSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USoundControlBusMix* GetDefaultControlBusMix() const { return DefaultControlBusMix.LoadSynchronous(); }
	USoundControlBusMix* GetLoadingScreenControlBusMix() const { return LoadingScreenControlBusMix.LoadSynchronous(); }
	USoundControlBusMix* GetUserSettingsControlBusMix() const { return UserSettingsControlBusMix.LoadSynchronous(); }
	USoundControlBus* GetOverallVolumeControlBus() const { return OverallVolumeControlBus.LoadSynchronous(); }
	USoundControlBus* GetMusicVolumeControlBus() const { return MusicVolumeControlBus.LoadSynchronous(); }
	USoundControlBus* GetSoundFXVolumeControlBus() const { return SoundFXVolumeControlBus.LoadSynchronous(); }

	UFMODVCA* GetMasterVCA() const { return FMOD_MasterVCA.LoadSynchronous(); }
	UFMODVCA* GetMusicVCA() const { return FMOD_MusicVCA.LoadSynchronous(); }
	UFMODVCA* GetSoundFxVCA() const { return FMOD_SoundFxVCA.LoadSynchronous(); }
	UFMODBus* GetMasterBus() const { return FMOD_MasterBus.LoadSynchronous(); }

private:
	UPROPERTY(EditAnywhere, Config, Category="Sound Mix")
	TSoftObjectPtr<USoundControlBusMix> DefaultControlBusMix;

	UPROPERTY(EditAnywhere, Config, Category="Sound Mix")
	TSoftObjectPtr<USoundControlBusMix> LoadingScreenControlBusMix;
	
	UPROPERTY(EditAnywhere, Config, Category="Sound Mix")
	TSoftObjectPtr<USoundControlBusMix> UserSettingsControlBusMix;
	
	UPROPERTY(EditAnywhere, Config, Category="Sound Mix")
	TSoftObjectPtr<USoundControlBus> OverallVolumeControlBus;

	UPROPERTY(EditAnywhere, Config, Category="Sound Mix")
	TSoftObjectPtr<USoundControlBus> MusicVolumeControlBus;

	UPROPERTY(EditAnywhere, Config, Category="Sound Mix")
	TSoftObjectPtr<USoundControlBus> SoundFXVolumeControlBus;

	UPROPERTY(EditAnywhere, Config, Category=FMOD, DisplayName="Master VCA")
	TSoftObjectPtr<UFMODVCA> FMOD_MasterVCA;

	UPROPERTY(EditAnywhere, Config, Category=FMOD, DisplayName="Music VCA")
	TSoftObjectPtr<UFMODVCA> FMOD_MusicVCA;

	UPROPERTY(EditAnywhere, Config, Category=FMOD, DisplayName="SoundFX VCA")
	TSoftObjectPtr<UFMODVCA> FMOD_SoundFxVCA;

	UPROPERTY(EditAnywhere, Config, Category=FMOD, DisplayName="Master Bus")
	TSoftObjectPtr<UFMODBus> FMOD_MasterBus;
};
