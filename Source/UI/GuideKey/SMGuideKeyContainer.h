// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMGuideKeyWidget.h"
#include "SMGuideKeyContainer.generated.h"

class UDynamicEntryBox;
/**
 * StereoMix Guide Keys Container
 */
UCLASS(Abstract, DisplayName = "Guide Key Container")
class STEREOMIX_API USMGuideKeyContainer : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void InitializeContainer();

	UPROPERTY(EditAnywhere, Category = "Guide Key")
	TSoftClassPtr<USMGuideKeyWidget> GuideKeyWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta=(BindWidget))
	TObjectPtr<UDynamicEntryBox> GuideKeyBox;
};
