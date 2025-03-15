// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Messaging/CommonGameDialog.h"
#include "SMConfirmationDialog.generated.h"

class USMButtonBase;
class UInputAction;
class UCommonBorder;
class UDynamicEntryBox;
class UCommonRichTextBlock;
class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName = "StereoMix Confirmation Dialog", ClassGroup = "StereoMix UI")
class STEREOMIX_API USMConfirmationDialog : public UCommonGameDialog
{
	GENERATED_BODY()

public:
	virtual void SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback) override;
	virtual void KillDialog() override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

protected:
	virtual void NativeOnInitialized() override;

	virtual void CloseConfirmationWindow(ECommonMessagingResult Result);

private:
	UFUNCTION()
	FEventReply HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	FCommonMessagingResultDelegate OnResultCallback;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TitleTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> DescriptionRichTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> ButtonsEntryBox;
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> CancelAction;

	UPROPERTY(Transient)
	TObjectPtr<USMButtonBase> FirstButton;
};
