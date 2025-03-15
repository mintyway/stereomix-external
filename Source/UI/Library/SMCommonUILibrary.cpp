// Copyright Studio Surround. All Rights Reserved.


#include "SMCommonUILibrary.h"

#include "CommonUserWidget.h"
#include "Input/CommonUIInputTypes.h"

void USMCommonUILibrary::RegisterBinding(UCommonUserWidget* InWidget, const UInputAction* InputAction, bool bShouldDisplayInActionBar, bool bShouldConsumeInput, const FInputActionExecutedDelegate& Callback, FUIActionBindingHandle& BindingHandle)
{
	check(InWidget);

	FBindUIActionArgs BindArgs(InputAction, FSimpleDelegate::CreateLambda([InputAction, Callback] {
		(void)Callback.ExecuteIfBound(InputAction->GetFName());
	}));
	BindArgs.bDisplayInActionBar = bShouldDisplayInActionBar;
	BindArgs.bConsumeInput = bShouldConsumeInput;

	BindingHandle = InWidget->RegisterUIActionBinding(BindArgs);
}

void USMCommonUILibrary::RegisterBindingWithActionTag(UCommonUserWidget* InWidget, const FUIActionTag& ActionTag, bool bShouldDisplayInActionBar, bool bShouldConsumeInput, const FInputActionTagExecutedDelegate& Callback, FUIActionBindingHandle& BindingHandle)
{
	check(InWidget);

	FBindUIActionArgs BindArgs(ActionTag, FSimpleDelegate::CreateLambda([ActionTag, Callback] {
		(void)Callback.ExecuteIfBound(ActionTag);
	}));
	BindArgs.bDisplayInActionBar = bShouldDisplayInActionBar;
	BindArgs.bConsumeInput = bShouldConsumeInput;

	BindingHandle = InWidget->RegisterUIActionBinding(BindArgs);
}

void USMCommonUILibrary::AddBinding(UCommonUserWidget* InWidget, FUIActionBindingHandle BindingHandle)
{
	check(InWidget);
	if (BindingHandle.IsValid())
	{
		InWidget->AddActionBinding(BindingHandle);
	}
}

void USMCommonUILibrary::RemoveBinding(UCommonUserWidget* InWidget, FUIActionBindingHandle BindingHandle)
{
	check(InWidget);
	if (BindingHandle.IsValid())
	{
		InWidget->RemoveActionBinding(BindingHandle);
	}
}
