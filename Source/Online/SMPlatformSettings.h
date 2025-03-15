// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SMPlatformSettings.generated.h"

class IOnlineSubsystem;

USTRUCT(BlueprintType, DisplayName = "StereoMix Platform Data")
struct STEREOMIX_API FSMPlatformData
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Platform")
	FName PlatformName;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Platform")
	TSoftObjectPtr<UTexture2D> Icon;

	IOnlineSubsystem* GetOnlineSubsystem() const;
};


UCLASS(Config=Game, DefaultConfig, DisplayName = "StereoMix Platform Settings")
class STEREOMIX_API USMPlatformSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Platform")
	TArray<FSMPlatformData> Platforms;
};
