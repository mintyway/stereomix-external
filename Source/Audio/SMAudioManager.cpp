// Copyright Studio Surround. All Rights Reserved.


#include "SMAudioManager.h"

#include "SMAudioSettings.h"
#include "SMFMODBlueprintStatics.h"
#include "Settings/SMSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMAudioManager)

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixAudioManager, Log, All);

USMAudioManager* USMAudioManager::Get(const UObject* ContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<USMAudioManager>();
		}
	}
	return nullptr;
}

bool USMAudioManager::ShouldCreateSubsystem(UObject* Outer) const
{
	UGameInstance* GameInstance = Cast<UGameInstance>(Outer);
	UWorld* World = GameInstance ? GameInstance->GetWorld() : nullptr;
	return World && World->IsGameWorld() && World->GetNetMode() != NM_DedicatedServer;
}

void USMAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MasterBus = GetDefault<USMAudioSettings>()->GetMasterBus();

	if (FSlateApplication::IsInitialized())
	{
		OnApplicationActivationStateChangedHandle =
			FSlateApplication::Get()
			.OnApplicationActivationStateChanged()
			.AddUObject(this, &ThisClass::OnAppActivationStateChanged);
	}
}

void USMAudioManager::Deinitialize()
{
	Super::Deinitialize();

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().Remove(OnApplicationActivationStateChangedHandle);
	}
}

void USMAudioManager::OnAppActivationStateChanged(bool bActivated) const
{
	if (const USMLocalPlayer* LocalPlayer = Cast<USMLocalPlayer>(GetGameInstance()->GetLocalPlayerByIndex(0)))
	{
		if (const USMSettingsShared* Settings = LocalPlayer->GetSharedSettings())
		{
			if (Settings->GetAllowAudioInBackgroundSetting() == ESMAllowBackgroundAudioSetting::Off)
			{
				if (MasterBus)
				{
					USMFMODBlueprintStatics::BusSetMute(MasterBus, !bActivated);
				}
			}
		}
	}
}
