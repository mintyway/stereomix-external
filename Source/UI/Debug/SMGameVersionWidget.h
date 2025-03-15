// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SMGameVersionWidget.generated.h"


class UTextBlock;

UCLASS(Abstract, DisplayName = "StereoMix Game Version Widget")
class STEREOMIX_API USMGameVersionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> VersionTextBlock;
};
