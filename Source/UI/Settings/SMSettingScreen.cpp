// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingScreen.h"

#include "StereoMixLog.h"
#include "Input/CommonUIInputTypes.h"
#include "Settings/SMGameSettingRegistry.h"
#include "UI/Library/SMCommonUILibrary.h"

USMSettingScreen::USMSettingScreen()
{
	bIsBackHandler = true;
}

void USMSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ApplyInputAction)
	{
		ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputAction, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	}
	else
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("ApplyInputAction is not set for %s"), *GetName());
	}

	if (CancelChangesInputAction)
	{
		CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputAction, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
	}
	else
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("CancelChangesInputAction is not set for %s"), *GetName());
	}
}

UGameSettingRegistry* USMSettingScreen::CreateRegistry()
{
	USMGameSettingRegistry* NewRegistry = NewObject<USMGameSettingRegistry>();
	if (USMLocalPlayer* LocalPlayer = CastChecked<USMLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}
	return NewRegistry;
}

bool USMSettingScreen::NativeOnHandleBackAction()
{
	if (!AttemptToPopNavigation())
	{
		ApplyChanges();

		if (bDeactivateOnBackAction)
		{
			if (UCommonActivatableWidget* ParentWidget = Cast<UCommonActivatableWidget>(USMCommonUILibrary::FindParentWidgetOfType(this, UCommonActivatableWidget::StaticClass())))
			{
				ParentWidget->DeactivateWidget();
			}
			else
			{
				DeactivateWidget();
			}
		}
	}
	return true;
}

void USMSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void USMSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void USMSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
