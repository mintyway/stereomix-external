// Copyright Studio Surround. All Rights Reserved.


#include "SMTextInputDialog.h"

#include "Components/EditableTextBox.h"
#include "Internationalization/StringTableRegistry.h"

#define LOCTABLE_ID "Game"

USMTextInputDialogDescriptor* USMTextInputDialogDescriptor::CreateConfirmationOk(const FText& Header, const FText& Body, const FText& Hint, const FString& TextValidatorPattern)
{
	USMTextInputDialogDescriptor* Descriptor = NewObject<USMTextInputDialogDescriptor>();
	Descriptor->Header = Header;
	Descriptor->Body = Body;
	Descriptor->Hint = Hint;
	Descriptor->TextValidatorPattern = TextValidatorPattern;

	FConfirmationDialogAction ConfirmAction;
	ConfirmAction.Result = ECommonMessagingResult::Confirmed;
	ConfirmAction.OptionalDisplayText = LOCTABLE(LOCTABLE_ID, "OK");

	Descriptor->ButtonActions.Add(ConfirmAction);

	return Descriptor;
}

USMTextInputDialogDescriptor* USMTextInputDialogDescriptor::CreateConfirmationOkCancel(const FText& Header, const FText& Body, const FText& Hint, const FString& TextValidatorPattern)
{
	USMTextInputDialogDescriptor* Descriptor = NewObject<USMTextInputDialogDescriptor>();
	Descriptor->Header = Header;
	Descriptor->Body = Body;
	Descriptor->Hint = Hint;
	Descriptor->TextValidatorPattern = TextValidatorPattern;

	FConfirmationDialogAction ConfirmAction;
	ConfirmAction.Result = ECommonMessagingResult::Confirmed;
	ConfirmAction.OptionalDisplayText = LOCTABLE(LOCTABLE_ID, "OK");

	FConfirmationDialogAction CancelAction;
	CancelAction.Result = ECommonMessagingResult::Cancelled;
	CancelAction.OptionalDisplayText = LOCTABLE(LOCTABLE_ID, "Cancel");

	Descriptor->ButtonActions.Add(ConfirmAction);
	Descriptor->ButtonActions.Add(CancelAction);

	return Descriptor;
}

void USMTextInputDialog::SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback)
{
	Super::SetupDialog(Descriptor, ResultCallback);

	USMTextInputDialogDescriptor* InputDescriptor = CastChecked<USMTextInputDialogDescriptor>(Descriptor);
	InputTextBox->SetHintText(InputDescriptor->Hint);
	InputTextBox->SetText(FText::GetEmpty());
	InputTextBox->OnTextChanged.AddUniqueDynamic(this, &ThisClass::NativeOnInputUpdated);
	TextValidator = FRegexPattern(InputDescriptor->TextValidatorPattern);
}

void USMTextInputDialog::KillDialog()
{
	Super::KillDialog();
}

void USMTextInputDialog::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void USMTextInputDialog::CloseConfirmationWindow(ECommonMessagingResult Result)
{
	if (Result == ECommonMessagingResult::Confirmed)
	{
		return;
	}
	Super::CloseConfirmationWindow(Result);
}

void USMTextInputDialog::NativeOnInputUpdated(const FText& InputText)
{
	BP_OnInputUpdated(InputText);
}

#undef LOCTABLE_ID
