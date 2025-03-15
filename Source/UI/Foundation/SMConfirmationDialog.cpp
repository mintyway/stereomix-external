// Copyright Studio Surround. All Rights Reserved.


#include "SMConfirmationDialog.h"

#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "ICommonInputModule.h"
#include "Components/DynamicEntryBox.h"
#include "UI/SMUIActionRouter.h"
#include "UI/Common/SMButtonBase.h"

void USMConfirmationDialog::SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback)
{
	TitleTextBlock->SetText(Descriptor->Header);
	DescriptionRichTextBlock->SetText(Descriptor->Body);

	ButtonsEntryBox->Reset<USMButtonBase>([](const USMButtonBase& Button) {
		Button.OnClicked().Clear();
	});

	FirstButton = nullptr;

	for (const FConfirmationDialogAction& Action : Descriptor->ButtonActions)
	{
		TObjectPtr<UInputAction> InputAction = nullptr;
		switch (Action.Result)
		{
			case ECommonMessagingResult::Confirmed:
				InputAction = ICommonInputModule::GetSettings().GetEnhancedInputClickAction();
				break;
			case ECommonMessagingResult::Declined:
				InputAction = ICommonInputModule::GetSettings().GetEnhancedInputBackAction();
				break;
			case ECommonMessagingResult::Cancelled:
				InputAction = CancelAction;
				break;
			default:
				ensure(false);
				continue;
		}

		USMButtonBase* Button = ButtonsEntryBox->CreateEntry<USMButtonBase>();
		Button->SetTriggeringEnhancedInputAction(InputAction);
		Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
		Button->SetButtonText(Action.OptionalDisplayText);

		if (!FirstButton)
		{
			FirstButton = Button;
		}
	}
	OnResultCallback = ResultCallback;
}

void USMConfirmationDialog::KillDialog()
{
}

TOptional<FUIInputConfig> USMConfirmationDialog::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, EMouseLockMode::DoNotLock, false);
}

UWidget* USMConfirmationDialog::NativeGetDesiredFocusTarget() const
{
	return FirstButton;
}

void USMConfirmationDialog::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USMConfirmationDialog::CloseConfirmationWindow(ECommonMessagingResult Result)
{
	DeactivateWidget();
	(void)OnResultCallback.ExecuteIfBound(Result);
}

FEventReply USMConfirmationDialog::HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = FReply::Unhandled();
	if (MouseEvent.IsTouchEvent() || MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CloseConfirmationWindow(ECommonMessagingResult::Declined);
		Reply.NativeReply = FReply::Handled();
	}
	return Reply;
}
