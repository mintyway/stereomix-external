// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMTeamStatics.generated.h"

enum class ESMTeam : uint8;
class USMTeamDefinition;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMTeamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Teams")
	static const USMTeamDefinition* GetTeamDefinition(ESMTeam Team);
};
