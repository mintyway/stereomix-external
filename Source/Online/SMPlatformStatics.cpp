// Copyright Studio Surround. All Rights Reserved.


#include "SMPlatformStatics.h"

#include "OnlineSubsystemUtils.h"
#include "SMPlatformSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixPlatform, Log, All);


bool USMPlatformStatics::TryFindDefaultPlatformData(FSMPlatformData& OutPlatformData)
{
	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(nullptr);
	if (!OnlineSub)
	{
		UE_LOG(LogStereoMixPlatform, Error, TEXT("Failed to get online subsystem."));
		return false;
	}

	const FName PlatformName = OnlineSub->GetSubsystemName();
	return TryFindPlatformData(PlatformName, OutPlatformData);
}

bool USMPlatformStatics::TryFindNativePlatformData(FSMPlatformData& OutPlatformData)
{
	const FName PlatformName = GetNativePlatformName();
	if (PlatformName.IsNone())
	{
		UE_LOG(LogStereoMixPlatform, Error, TEXT("Failed to get native platform name."));
		return false;
	}
	return TryFindPlatformData(PlatformName, OutPlatformData);
}

bool USMPlatformStatics::TryFindPlatformData(const FName& PlatformName, FSMPlatformData& OutPlatformData)
{
	const USMPlatformSettings* PlatformSettings = GetDefault<USMPlatformSettings>();
	const FSMPlatformData* Platform = PlatformSettings->Platforms.FindByPredicate([PlatformName](const FSMPlatformData& PlatformData) {
		return PlatformData.PlatformName == PlatformName;
	});

	if (!Platform)
	{
		UE_LOG(LogStereoMixPlatform, Error, TEXT("Failed to find platform data for %s."), *PlatformName.ToString());
		return false;
	}

	OutPlatformData = *Platform;
	return true;
}

UTexture2D* USMPlatformStatics::GetSpecificPlatformIcon(const FName& PlatformName)
{
	const USMPlatformSettings* PlatformSettings = GetDefault<USMPlatformSettings>();
	const FSMPlatformData* Platform = PlatformSettings->Platforms.FindByPredicate([PlatformName](const FSMPlatformData& PlatformData) {
		return PlatformData.PlatformName == PlatformName;
	});

	if (!Platform)
	{
		UE_LOG(LogStereoMixPlatform, Error, TEXT("Failed to find platform data for %s."), *PlatformName.ToString());
		return nullptr;
	}

	return Platform->Icon.LoadSynchronous();
}

FName USMPlatformStatics::GetNativePlatformName()
{
	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::GetByPlatform();
	if (!OnlineSub)
	{
		UE_LOG(LogStereoMixPlatform, Log, TEXT("Failed to get platform online subsystem."));
		return NAME_None;
	}

	return OnlineSub->GetSubsystemName();
}