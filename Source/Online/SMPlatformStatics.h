// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMPlatformSettings.h"
#include "SMPlatformStatics.generated.h"

UCLASS()
class USMPlatformStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="StereoMix|Platform")
	static bool TryFindDefaultPlatformData(FSMPlatformData& OutPlatformData);
	
	UFUNCTION(BlueprintCallable, Category="StereoMix|Platform")
	static bool TryFindNativePlatformData(FSMPlatformData& OutPlatformData);
	
	UFUNCTION(BlueprintCallable, Category="StereoMix|Platform")
	static bool TryFindPlatformData(const FName& PlatformName, FSMPlatformData& OutPlatformData);
	
	UFUNCTION(BlueprintCallable, Category="StereoMix|Platform")
	static UTexture2D* GetSpecificPlatformIcon(const FName& PlatformName);

	UFUNCTION(BlueprintPure, Category="StereoMix|Platform")
	static FName GetNativePlatformName();
};
