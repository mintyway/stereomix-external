// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMGuideKeyWidget.generated.h"

class UTextBlock;
class UCommonActionWidget;
class UInputAction;

/**
 * StereoMix Guide Key Widget
 */
UCLASS(Abstract, DisplayName = "Guide Key Widget")
class STEREOMIX_API USMGuideKeyWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void InitializeAction(const UInputAction* InAction);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Guide Key", meta = (BindWidget))
	TObjectPtr<UCommonActionWidget> InputActionWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Guide Key", meta = (BindWidget))
	TObjectPtr<UTextBlock> DisplayTextBlock;
};
