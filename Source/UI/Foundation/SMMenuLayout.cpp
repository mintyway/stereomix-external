// Copyright Studio Surround. All Rights Reserved.


#include "SMMenuLayout.h"

#include "PrimaryGameLayout.h"
#include "SMGameplayTags.h"
#include "UI/Foundation/SMBackgroundScreen.h"

using namespace StereoMix;

void USMMenuLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USMMenuLayout::NativeConstruct()
{
	Super::NativeConstruct();
	InitBackground();
}

void USMMenuLayout::NativeDestruct()
{
	Super::NativeDestruct();
}

void USMMenuLayout::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void USMMenuLayout::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void USMMenuLayout::InitBackground()
{
	if (!bShowBackground)
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
		{
			if (UCommonActivatableWidgetContainerBase* MenuBackgroundLayer = RootLayout->GetLayerWidget(Tags::UI_Layer_MenuBackground))
			{
				MenuBackgroundLayer->ClearWidgets();
			}
		}
		return;
	}

	if (!BackgroundClass.ToSoftObjectPath().IsValid())
	{
		return;
	}
	SetBackground(BackgroundClass.LoadSynchronous());
}

void USMMenuLayout::SetBackground(const TSubclassOf<USMBackgroundScreen>& Background) const
{
	if (!BackgroundClass.IsValid())
	{
		return;
	}


	UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayer());
	if (!RootLayout)
	{
		return;
	}

	UCommonActivatableWidgetContainerBase* BackgroundLayer = RootLayout->GetLayerWidget(Tags::UI_Layer_MenuBackground);
	if (!ensure(BackgroundLayer))
	{
		return;
	}

	if (UCommonActivatableWidget* ActiveBackgroundWidget = BackgroundLayer->GetActiveWidget())
	{
		if (ActiveBackgroundWidget->GetClass() == Background)
		{
			USMBackgroundScreen* BackgroundWidget = CastChecked<USMBackgroundScreen>(ActiveBackgroundWidget);
			BackgroundWidget->InitializeBackground(false);
			return;
		}
		ActiveBackgroundWidget->DeactivateWidget();
	}

	RootLayout->PushWidgetToLayerStack<USMBackgroundScreen>(Tags::UI_Layer_MenuBackground, Background, [](USMBackgroundScreen& NewBackground) {
		NewBackground.InitializeBackground(true);
	});
}
