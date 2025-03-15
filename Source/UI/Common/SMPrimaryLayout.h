// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMActivatableWidget.h"
#include "SMPrimaryLayout.generated.h"


UCLASS(Abstract, DisplayName = "StereoMix Primary Layout", Category = "StereoMix|UI")
class STEREOMIX_API USMPrimaryLayout : public USMActivatableWidget
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TEnumAsByte<EMouseCursor::Type> DefaultMouseCursor = EMouseCursor::Type::Default;
};
