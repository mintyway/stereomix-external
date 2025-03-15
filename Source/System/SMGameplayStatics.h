// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "SMGameplayStatics.generated.h"


UCLASS(MinimalAPI)
class USMGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "StereoMix")
	static STEREOMIX_API FString GetProjectVersion();

	UFUNCTION(BlueprintPure, Category = "StereoMix")
	static STEREOMIX_API FString GetBuildVersion();
};
