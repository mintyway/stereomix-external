// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Data/DataTable/Tutorial/SMUserProfileDataTable.h"
#include "SMUserSettings.generated.h"

/**
 * StereoMix User Settings
 */
UCLASS(Config=Game, DefaultConfig, DisplayName="StereoMix User Settings")
class STEREOMIX_API USMUserSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UDataTable* GetUserProfileDataTable() const { return UserProfileDataTable.LoadSynchronous(); }

protected:
	UPROPERTY(Config, EditAnywhere, Category="User", DisplayName="User Profiles")
	TSoftObjectPtr<UDataTable> UserProfileDataTable;
};
