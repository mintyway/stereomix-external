// Copyright Studio Surround. All Rights Reserved.


#include "SMAudioMixEffectsSubsystem.h"

#include "AudioModulationStatics.h"
#include "LoadingScreenManager.h"
#include "SMAudioSettings.h"
#include "Settings/SMSettingsLocal.h"

void USMAudioMixEffectsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USMAudioMixEffectsSubsystem::PostInitialize()
{
	Super::PostInitialize();

	if (const USMAudioSettings* AudioSettings = GetDefault<USMAudioSettings>())
	{
		if (USoundControlBusMix* SoundControlBusMix = AudioSettings->GetDefaultControlBusMix())
		{
			DefaultBaseMix = SoundControlBusMix;
		}
		else
		{
			ensureMsgf(SoundControlBusMix, TEXT("Default Control Bus Mix reference missing from StereoMix Audio Settings."));
		}

		if (USoundControlBusMix* SoundControlBusMix = AudioSettings->GetLoadingScreenControlBusMix())
		{
			LoadingScreenMix = SoundControlBusMix;
		}
		else
		{
			ensureMsgf(SoundControlBusMix, TEXT("Loading Screen Control Bus Mix reference missing from StereoMix Audio Settings."));
		}

		if (USoundControlBusMix* SoundControlBusMix = AudioSettings->GetUserSettingsControlBusMix())
		{
			UserMix = SoundControlBusMix;
		}
		else
		{
			ensureMsgf(SoundControlBusMix, TEXT("User Settings Control Bus Mix reference missing from StereoMix Audio Settings."));
		}

		if (USoundControlBus* SoundControlBus = AudioSettings->GetOverallVolumeControlBus())
		{
			OverallControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("Overall Control Bus reference missing from StereoMix Audio Settings."));
		}

		if (USoundControlBus* SoundControlBus = AudioSettings->GetMusicVolumeControlBus())
		{
			MusicControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("Music Control Bus reference missing from StereoMix Audio Settings."));
		}

		if (USoundControlBus* SoundControlBus = AudioSettings->GetSoundFXVolumeControlBus())
		{
			SoundFXControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("SoundFX Control Bus reference missing from StereoMix Audio Settings."));
		}
	}

	// Register with the loading screen manager
	if (ULoadingScreenManager* LoadingScreenManager = UGameInstance::GetSubsystem<ULoadingScreenManager>(GetWorld()->GetGameInstance()))
	{
		LoadingScreenManager->OnLoadingScreenVisibilityChangedDelegate().AddUObject(this, &ThisClass::OnLoadingScreenStatusChanged);
		ApplyOrRemoveLoadingScreenMix(LoadingScreenManager->GetLoadingScreenDisplayStatus());
	}
}

void USMAudioMixEffectsSubsystem::Deinitialize()
{
	if (ULoadingScreenManager* LoadingScreenManager = UGameInstance::GetSubsystem<ULoadingScreenManager>(GetWorld()->GetGameInstance()))
	{
		LoadingScreenManager->OnLoadingScreenVisibilityChangedDelegate().RemoveAll(this);
		ApplyOrRemoveLoadingScreenMix(false);
	}
	Super::Deinitialize();
}

bool USMAudioMixEffectsSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	bool bShouldCreateSubsystem = Super::ShouldCreateSubsystem(Outer);

	if (Outer)
	{
		if (UWorld* World = Outer->GetWorld())
		{
			bShouldCreateSubsystem = DoesSupportWorldType(World->WorldType) && bShouldCreateSubsystem;
		}
	}

	return bShouldCreateSubsystem;
}

void USMAudioMixEffectsSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	if (const UWorld* World = GetWorld())
	{
		if (DefaultBaseMix)
		{
			UAudioModulationStatics::ActivateBusMix(World, DefaultBaseMix);
		}

		if (const USMSettingsLocal* LocalSettings = USMSettingsLocal::Get())
		{
			if (UserMix)
			{
				UAudioModulationStatics::ActivateBusMix(World, UserMix);

				if (OverallControlBus && MusicControlBus && SoundFXControlBus)
				{
					const FSoundControlBusMixStage OverallControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, OverallControlBus, LocalSettings->GetOverallVolume());
					const FSoundControlBusMixStage MusicControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, MusicControlBus, LocalSettings->GetMusicVolume());
					const FSoundControlBusMixStage SoundFXControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, SoundFXControlBus, LocalSettings->GetSoundFXVolume());

					TArray<FSoundControlBusMixStage> ControlBusMixStages;
					ControlBusMixStages.Add(OverallControlBusMixStage);
					ControlBusMixStages.Add(MusicControlBusMixStage);
					ControlBusMixStages.Add(SoundFXControlBusMixStage);

					UAudioModulationStatics::UpdateMix(World, UserMix, ControlBusMixStages);
				}
			}
		}
	}
}

bool USMAudioMixEffectsSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// We only need this subsystem on Game worlds (PIE included)
	return (WorldType == EWorldType::Game || WorldType == EWorldType::PIE);
}

void USMAudioMixEffectsSubsystem::OnLoadingScreenStatusChanged(bool bShowingLoadingScreen)
{
	ApplyOrRemoveLoadingScreenMix(bShowingLoadingScreen);
}

void USMAudioMixEffectsSubsystem::ApplyOrRemoveLoadingScreenMix(bool bWantsLoadingScreenMix)
{
	UWorld* World = GetWorld();

	if (bAppliedLoadingScreenMix != bWantsLoadingScreenMix && LoadingScreenMix && World)
	{
		if (bWantsLoadingScreenMix)
		{
			// Apply the mix
			UAudioModulationStatics::ActivateBusMix(World, LoadingScreenMix);
		}
		else
		{
			// Remove the mix
			UAudioModulationStatics::DeactivateBusMix(World, LoadingScreenMix);
		}
		bAppliedLoadingScreenMix = bWantsLoadingScreenMix;
	}
}
