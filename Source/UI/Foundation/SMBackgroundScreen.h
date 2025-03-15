// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMBackgroundScreen.generated.h"

/**
 * 
 */
UCLASS(Abstract, DisplayName = "StereoMix Background Screen", ClassGroup = "StereoMix UI")
class STEREOMIX_API USMBackgroundScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void InitializeBackground(bool bIsNew);

	UFUNCTION(BlueprintImplementableEvent, Category = "Background", DisplayName = "Initialize Background")
	void BP_InitializeBackground(bool bIsNew);
};
