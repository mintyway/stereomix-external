// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/SMUserWidget.h"
#include "SMEmoticonWidget.generated.h"


class UOverlay;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMEmoticonWidget : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void PlayHiEmoticon();

	void PlayGoodEmoticon();

	void PlaySadEmoticon();

	void PreparePlayEmoticonAnimation();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HiAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GoodAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SadAnimation;
};
