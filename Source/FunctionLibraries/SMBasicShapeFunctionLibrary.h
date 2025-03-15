// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "SMBasicShapeFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMBasicShapeFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static UStaticMesh* GetPlaneMesh();
};
