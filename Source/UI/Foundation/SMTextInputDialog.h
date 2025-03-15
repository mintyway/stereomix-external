// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMConfirmationDialog.h"
#include "Internationalization/Regex.h"
#include "SMTextInputDialog.generated.h"

class UEditableTextBox;
class FRegexPattern;

DECLARE_DELEGATE_TwoParams(FCommonInputMessagingResultDelegate, ECommonMessagingResult /* Result */, FText /* InputText */);

UCLASS(DisplayName = "StereoMix Text Input Dialog", ClassGroup = "StereoMix UI")
class STEREOMIX_API USMTextInputDialogDescriptor : public UCommonGameDialogDescriptor
{
	GENERATED_BODY()

public:
	static USMTextInputDialogDescriptor* CreateConfirmationOk(const FText& Header, const FText& Body, const FText& Hint, const FString& TextValidatorPattern);
	static USMTextInputDialogDescriptor* CreateConfirmationOkCancel(const FText& Header, const FText& Body, const FText& Hint, const FString& TextValidatorPattern);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Hint;
	
	FString TextValidatorPattern;
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class STEREOMIX_API USMTextInputDialog : public USMConfirmationDialog
{
	GENERATED_BODY()

public:
	virtual void SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback) override;
	virtual void KillDialog() override;

protected:
	virtual void NativeOnActivated() override;
	virtual void CloseConfirmationWindow(ECommonMessagingResult Result) override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnInputUpdated")
	void BP_OnInputUpdated(const FText& InputText);

	UFUNCTION()
	virtual void NativeOnInputUpdated(const FText& InputText);

private:
	FCommonInputMessagingResultDelegate OnInputResultCallback;
	FRegexPattern TextValidator = FRegexPattern(TEXT(".*"));

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> InputTextBox;
};
