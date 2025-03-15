// Copyright Studio Surround. All Rights Reserved.


#include "SMBoundActionButton.h"

#include "CommonActionWidget.h"
#include "CommonInputSubsystem.h"
#include "CommonTextBlock.h"
#include "CommonUITypes.h"
#include "Input/UIActionBinding.h"
#include "UI/SMUITypes.h"

void USMBoundActionButton::SetRepresentedAction(FUIActionBindingHandle InBindingHandle)
{
	if (TSharedPtr<FUIActionBinding> OldBinding = FUIActionBinding::FindBinding(BindingHandle))
	{
		OldBinding->OnHoldActionProgressed.RemoveAll(this);
	}

	BindingHandle = InBindingHandle;
	UpdateInputActionWidget();

	if (TSharedPtr<FUIActionBinding> NewBinding = FUIActionBinding::FindBinding(InBindingHandle))
	{
		NewBinding->OnHoldActionProgressed.AddUObject(this, &ThisClass::NativeOnActionProgress);
	}
}

void USMBoundActionButton::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITORONLY_DATA
	if (IsDesignTime() && bPreview)
	{
		UpdateButtonStyle(PreviewInputType);
	}
#endif
}

void USMBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();
	auto InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer());
	ECommonInputType CurrentInputType = InputSubsystem->GetCurrentInputType();
	UpdateButtonStyle(CurrentInputType);
}

void USMBoundActionButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	if (TSharedPtr<FUIActionBinding> ActionBinding = FUIActionBinding::FindBinding(BindingHandle))
	{
		(void)ActionBinding->OnExecuteAction.ExecuteIfBound();
	}
}

void USMBoundActionButton::UpdateInputActionWidget()
{
	if (InputActionWidget)
	{
		auto ActionBinding = FUIActionBinding::FindBinding(BindingHandle);

		if (ActionBinding.IsValid() && ActionBinding->InputAction.IsValid())
		{
			UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer());
			ECommonInputType InputType = InputSubsystem->GetCurrentInputType();
			SetVisibility(CommonUI::ActionValidForInputType(GetOwningLocalPlayer(),
				              InputType,
				              ActionBinding->InputAction.Get())
				              ? InputType == ECommonInputType::MouseAndKeyboard
					                ? ESlateVisibility::Visible
					                : ESlateVisibility::HitTestInvisible
				              : ESlateVisibility::Collapsed);
		}
		InputActionWidget->SetInputActionBinding(BindingHandle);
		FText ActionDisplayName = BindingHandle.GetDisplayName();
		if (DisplayTextBlock)
		{
			DisplayTextBlock->SetText(ActionDisplayName);
		}
		OnUpdateInputAction();
	}
}

void USMBoundActionButton::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	UpdateInputActionWidget();
	UpdateButtonStyle(CurrentInputType);
	Super::OnInputMethodChanged(CurrentInputType);
}

void USMBoundActionButton::UpdateButtonStyle(ECommonInputType InputType)
{
	TSubclassOf<UCommonButtonStyle> NewStyle;
	if (InputType == ECommonInputType::Gamepad)
	{
		NewStyle = GamepadStyle;
	}
	else if (InputType == ECommonInputType::Touch)
	{
		NewStyle = TouchStyle;
	}
	else
	{
		NewStyle = KeyboardStyle;
	}

	if (NewStyle)
	{
		SetStyle(NewStyle);
	}
}
