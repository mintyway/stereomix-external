// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMButtonBase.h"
#include "SMBoundActionButton.generated.h"

/**
 * StereoMix Bound Action Button
 */
UCLASS(Abstract, DisplayName = "StereoMix Bound Action Button", meta = (DisableNativeTick))
class STEREOMIX_API USMBoundActionButton : public USMButtonBase, public ICommonBoundActionButtonInterface
{
	GENERATED_BODY()

	virtual void SetRepresentedAction(FUIActionBindingHandle InBindingHandle) override;
	
protected:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeOnClicked() override;
	
	virtual void UpdateInputActionWidget() override;
	
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;

	void UpdateButtonStyle(ECommonInputType InputType);

	UFUNCTION(BlueprintImplementableEvent, Category = "StereoMix Bound Action")
	void OnUpdateInputAction();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Text Block")
	TObjectPtr<UCommonTextBlock> DisplayTextBlock;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Preview", meta = (InlineEditConditionToggle))
	uint8 bPreview : 1 = false;
	
	UPROPERTY(EditAnywhere, Category = "Preview", meta = (EditCondition = "bPreview"))
	ECommonInputType PreviewInputType = ECommonInputType::Gamepad;
#endif
	
private:
	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> TouchStyle;

	FUIActionBindingHandle BindingHandle;
};
