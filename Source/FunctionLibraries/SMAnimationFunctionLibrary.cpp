// Copyright Studio Surround. All Rights Reserved.


#include "SMAnimationFunctionLibrary.h"

float USMAnimationFunctionLibrary::GetSectionLengthWithRate(const UAnimMontage* Montage, FName SectionName, TOptional<float> RateScaleOption)
{
	if (!Montage)
	{
		return 0.0f;
	}

	const float RateScale = RateScaleOption.Get(Montage->RateScale);
	return Montage->GetSectionLength(Montage->GetSectionIndex(SectionName)) / RateScale;
}
