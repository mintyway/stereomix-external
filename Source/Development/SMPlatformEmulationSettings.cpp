﻿// Copyright Studio Surround. All Rights Reserved.


#include "SMPlatformEmulationSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "CommonUIVisibilitySubsystem.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

USMPlatformEmulationSettings::USMPlatformEmulationSettings()
{
}

FName USMPlatformEmulationSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

FName USMPlatformEmulationSettings::GetPretendBaseDeviceProfile() const
{
	return PretendBaseDeviceProfile;
}

FName USMPlatformEmulationSettings::GetPretendPlatformName() const
{
	return PretendPlatform;
}

TArray<FName> USMPlatformEmulationSettings::GetKnownPlatformIds() const
{
	TArray<FName> Results;

#if WITH_EDITOR
	Results.Add(NAME_None);
	Results.Append(UPlatformSettingsManager::GetKnownAndEnablePlatformIniNames());
#endif

	return Results;
}

TArray<FName> USMPlatformEmulationSettings::GetKnownDeviceProfiles() const
{
	TArray<FName> Results;

#if WITH_EDITOR
	const UDeviceProfileManager& Manager = UDeviceProfileManager::Get();
	Results.Reserve(Manager.Profiles.Num() + 1);

	if (PretendPlatform == NAME_None)
	{
		Results.Add(NAME_None);
	}

	for (const TObjectPtr<UDeviceProfile>& Profile : Manager.Profiles)
	{
		bool bIncludeEntry = true;
		if (PretendPlatform != NAME_None)
		{
			if (Profile->DeviceType != PretendPlatform.ToString())
			{
				bIncludeEntry = false;
			}
		}
		if (bIncludeEntry)
		{
			Results.Add(Profile->GetFName());
		}
	}
#endif

	return Results;
}

void USMPlatformEmulationSettings::PickReasonableBaseDeviceProfile()
{
	// First see if our pretend device profile is already compatible, if so we don't need to do anything
	UDeviceProfileManager& Manager = UDeviceProfileManager::Get();
	if (UDeviceProfile* ProfilePtr = Manager.FindProfile(PretendBaseDeviceProfile.ToString(), /*bCreateOnFail=*/ false))
	{
		const bool bIsCompatible = (PretendPlatform == NAME_None) || (ProfilePtr->DeviceType == PretendPlatform.ToString());
		if (!bIsCompatible)
		{
			PretendBaseDeviceProfile = NAME_None;
		}
	}

	if ((PretendPlatform != NAME_None) && (PretendBaseDeviceProfile == NAME_None))
	{
		// If we're pretending we're a platform and don't have a pretend base profile, pick a reasonable one,
		// preferring the one with the shortest name as a simple heuristic
		FName ShortestMatchingProfileName;
		const FString PretendPlatformStr = PretendPlatform.ToString();
		for (const TObjectPtr<UDeviceProfile>& Profile : Manager.Profiles)
		{
			if (Profile->DeviceType == PretendPlatformStr)
			{
				const FName TestName = Profile->GetFName();
				if ((ShortestMatchingProfileName == NAME_None) || (TestName.GetStringLength() < ShortestMatchingProfileName.GetStringLength()))
				{
					ShortestMatchingProfileName = TestName;
				}
			}
		}
		PretendBaseDeviceProfile = ShortestMatchingProfileName;
	}
}

#if WITH_EDITOR

void USMPlatformEmulationSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplySettings();
}

void USMPlatformEmulationSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	ApplySettings();
}

void USMPlatformEmulationSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ApplySettings();
}

void USMPlatformEmulationSettings::OnPlayInEditorStarted() const
{
	// Show a notification toast to remind the user that there's a tag enable override set
	if (!AdditionalPlatformTraitsToEnable.IsEmpty())
	{
		FNotificationInfo Info(FText::Format(
			FText::FromString(TEXT("Platform Trait Override\nEnabling {0}")),
			FText::AsCultureInvariant(AdditionalPlatformTraitsToEnable.ToStringSimple())
			));
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}

	// Show a notification toast to remind the user that there's a tag suppression override set
	if (!AdditionalPlatformTraitsToSuppress.IsEmpty())
	{
		FNotificationInfo Info(FText::Format(
			FText::FromString(TEXT("Platform Trait Override\nSuppressing {0}")),
			FText::AsCultureInvariant(AdditionalPlatformTraitsToSuppress.ToStringSimple())
			));
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}

	// Show a notification toast to remind the user that there's a platform override set
	if (PretendPlatform != NAME_None)
	{
		FNotificationInfo Info(FText::Format(
			FText::FromString(TEXT("Platform Override Active\nPretending to be {0}")),
			FText::FromName(PretendPlatform)
			));
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

void USMPlatformEmulationSettings::ApplySettings()
{
	UCommonUIVisibilitySubsystem::SetDebugVisibilityConditions(AdditionalPlatformTraitsToEnable, AdditionalPlatformTraitsToSuppress);
	if (GIsEditor && PretendPlatform != LastAppliedPretendPlatform)
	{
		ChangeActivePretendPlatform(PretendPlatform);
	}
	PickReasonableBaseDeviceProfile();
}

void USMPlatformEmulationSettings::ChangeActivePretendPlatform(FName NewPlatformName)
{
	LastAppliedPretendPlatform = NewPlatformName;
	PretendPlatform = NewPlatformName;
	UPlatformSettingsManager::SetEditorSimulatedPlatform(PretendPlatform);
}

#endif