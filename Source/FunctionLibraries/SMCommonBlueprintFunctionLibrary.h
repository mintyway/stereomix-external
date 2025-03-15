// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMCommonBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCommonBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SMCommon")
	static bool IsEditor();
};
