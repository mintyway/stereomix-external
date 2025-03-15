// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "SMAnimationFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAnimationFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static float GetSectionLengthWithRate(const UAnimMontage* Montage, FName SectionName, TOptional<float> RateScaleOption = TOptional<float>());
};
