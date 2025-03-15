// Copyright Studio Surround. All Rights Reserved.


#include "SMLocalPlayer.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Settings/SMSettingsLocal.h"
#include "Settings/SMSettingsShared.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixLocalPlayer, Log, All);

USMLocalPlayer::USMLocalPlayer()
{
}

void USMLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();
	if (USMSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioDeviceChanged().AddUObject(this, &ThisClass::OnAudioOutputDeviceChanged);
	}
}

void USMLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);
	OnPlayerControllerChanged(PlayerController);
}

bool USMLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);
	OnPlayerControllerChanged(PlayerController);
	return bResult;
}

void USMLocalPlayer::InitOnlineSession()
{
	OnPlayerControllerChanged(PlayerController);
	Super::InitOnlineSession();
}

USMSettingsLocal* USMLocalPlayer::GetLocalSettings() const
{
	return USMSettingsLocal::Get();
}

USMSettingsShared* USMLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		// On PC it's okay to use the sync load because it only checks the disk
		// This could use a platform tag to check for proper save support instead
		bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;
		if (bCanLoadBeforeLogin)
		{
			SharedSettings = USMSettingsShared::LoadOrCreateSettings(this);
		}
		else
		{
			// We need to wait for user login to get the real settings so return temp ones
			SharedSettings = USMSettingsShared::CreateTemporarySettings(this);
		}
	}
	return SharedSettings;
}

void USMLocalPlayer::LoadSharedSettingsFromDisk(bool bForceLoad)
{
	FUniqueNetIdRepl CurrentNetId = GetCachedUniqueNetId();
	if (!bForceLoad && SharedSettings && CurrentNetId == NetIdForSharedSettings)
	{
		// 이미 로드되어 있기 때문에 로드하지 않음
		return;
	}
	ensure(USMSettingsShared::AsyncLoadOrCreateSettings(this,USMSettingsShared::FSMOnSettingsLoaded::CreateUObject(this, &ThisClass::OnSharedSettingsLoaded)));
}

void USMLocalPlayer::OnSharedSettingsLoaded(USMSettingsShared* LoadedOrCreatedSettings)
{
	if (ensure(LoadedOrCreatedSettings))
	{
		SharedSettings = LoadedOrCreatedSettings;
		NetIdForSharedSettings = GetCachedUniqueNetId();
	}
}

void USMLocalPlayer::OnAudioOutputDeviceChanged(const FString& NewAudioOutputDevice)
{
	UFMODBlueprintStatics::SetOutputDriverByName(NewAudioOutputDevice);

	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), NewAudioOutputDevice, DevicesSwappedCallback);
}

void USMLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
}

void USMLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{
}
