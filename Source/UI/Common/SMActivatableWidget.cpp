// Copyright Studio Surround. All Rights Reserved.


#include "SMActivatableWidget.h"

#include "SMButtonBase.h"
#include "StereoMixLog.h"
#include "Animation/UMGSequencePlayer.h"
#include "Editor/WidgetCompilerLog.h"
#include "Input/CommonUIInputTypes.h"

void USMActivatableWidget::PerformBack()
{
	NativeOnHandleBackAction();
}

TOptional<FUIInputConfig> USMActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
		case ESMWidgetInputMode::GameAndMenu:
			return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode, GameMouseLockMode, false);
		case ESMWidgetInputMode::Game:
			return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode, GameMouseLockMode, false);
		case ESMWidgetInputMode::Menu:
			return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
		case ESMWidgetInputMode::Default:
		default:
			return TOptional<FUIInputConfig>();
	}
}

void USMActivatableWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	OnActivated().AddUObject(this, &ThisClass::HandleVisibilityBindableWidgets);
	OnDeactivated().AddUObject(this, &ThisClass::HandleVisibilityBindableWidgets);
}

void USMActivatableWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	// 최초 한번 VisibilityBindableWidgets의 상태를 업데이트함
	HandleVisibilityBindableWidgets();
}

void USMActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void USMActivatableWidget::NativeOnDeactivated()
{
	if (bUseCustomDeactivate && ensure(!IsActivated()))
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("This widget is using custom deactivate logic, skipping default deactivate logic."));
		NativeOnCustomDeactivateRequested();
		return;
	}
	Super::NativeOnDeactivated();
}

void USMActivatableWidget::HandleVisibilityBindableWidgets()
{
	for (auto BindableWidget : VisibilityBindableWidgets)
	{
		if (UWidget* TargetWidget = BindableWidget.Widget.Resolve(WidgetTree))
		{
			TargetWidget->SetVisibility(IsActivated() ? BindableWidget.ActivateVisibility : BindableWidget.DeactivateVisibility);
		}
	}
}

void USMActivatableWidget::NativeOnCustomDeactivateRequested()
{
	BP_OnCustomDeactivateRequested();
}

void USMActivatableWidget::FinishCustomDeactivate()
{
	if (bUseCustomDeactivate && ensure(!IsActivated()))
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("Finish custom deactivate logic. Execute default deactivate logic."))
		Super::NativeOnDeactivated();
	}
}

#if WITH_EDITOR
void USMActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(USMActivatableWidget, BP_GetDesiredFocusTarget)) && !GetDesiredFocusWidget())
	{
		if (GetParentNativeClass(GetClass()) == StaticClass())
		{
			CompileLog.Warning(FText::FromString(TEXT("GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.")));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			CompileLog.Note(FText::FromString(TEXT(
				"GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen. "
				"If it was implemented in the native base class you can ignore this message."
				)));
		}
	}
}
#endif
