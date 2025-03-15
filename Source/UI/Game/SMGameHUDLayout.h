// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/SMActivatableWidget.h"
#include "UI/Common/SMPrimaryLayout.h"
#include "SMGameHUDLayout.generated.h"


UCLASS(Abstract, meta=(DisplayName = "StereoMix Game HUD Layout", Category = "StereoMix|UI"))
class STEREOMIX_API USMGameHUDLayout : public USMPrimaryLayout
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	void HandleEscapeAction() const;

	/** Escape 버튼 클릭 시 보여지는 일시정지 메뉴 위젯 */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> EscapeAction;
};
