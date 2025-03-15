// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMWidgetUtilityFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMWidgetUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "StereoMix|UI")
	static bool IsBrushSet(const FSlateBrush& InBrush)
	{
		return InBrush.IsSet();
	}
};
