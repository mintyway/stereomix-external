// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionBar.h"
#include "SMBoundActionBar.generated.h"


UCLASS(DisplayName = "StereoMix Bound Action Bar")
class STEREOMIX_API USMBoundActionBar : public UCommonBoundActionBar
{
	GENERATED_BODY()

public:

protected:
	virtual UUserWidget* CreateActionButton(const FUIActionBindingHandle& BindingHandle) override;
};
