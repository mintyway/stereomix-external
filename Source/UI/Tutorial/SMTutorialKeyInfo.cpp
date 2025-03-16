// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialKeyInfo.h"

#include "Components/Overlay.h"


USMTutorialKeyInfo::USMTutorialKeyInfo() {}

void USMTutorialKeyInfo::NativeConstruct()
{
	Super::NativeConstruct();

	Overlays.Add(LeftClickOverlay);
	Overlays.Add(RightClickOverlay);
	Overlays.Add(RightLeftClickOverlay);
	Overlays.Add(WASDOverlay);
	Overlays.Add(EOverlay);

	HideAll();
}

void USMTutorialKeyInfo::HideAll()
{
	for (UWidget* Overlay : Overlays)
	{
		if (Overlay)
		{
			Overlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

bool USMTutorialKeyInfo::ShowLeftClick_Implementation()
{
	HideAll();

	if (LeftClickOverlay)
	{
		LeftClickOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		return true;
	}

	return false;
}

bool USMTutorialKeyInfo::ShowRightClick_Implementation()
{
	HideAll();

	if (RightClickOverlay)
	{
		RightClickOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		return true;
	}

	return false;
}

bool USMTutorialKeyInfo::ShowRightLeftClick_Implementation(bool bUseDimRightClick, bool bUseDimLeftClick)
{
	HideAll();

	if (RightLeftClickOverlay)
	{
		RightLeftClickOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		return true;
	}

	return false;
}

bool USMTutorialKeyInfo::ShowWASDKey_Implementation()
{
	HideAll();

	if (WASDOverlay)
	{
		WASDOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		return true;
	}

	return false;
}

bool USMTutorialKeyInfo::ShowEKey_Implementation()
{
	HideAll();

	if (EOverlay)
	{
		EOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		return true;
	}

	return false;
}

void USMTutorialKeyInfo::InternalHideLeftClick() const
{
	if (LeftClickOverlay)
	{
		LeftClickOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USMTutorialKeyInfo::InternalHideRightClick() const
{
	if (RightClickOverlay)
	{
		RightClickOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USMTutorialKeyInfo::InternalHideRightLeftClick() const
{
	if (RightLeftClickOverlay)
	{
		RightLeftClickOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USMTutorialKeyInfo::InternalHideWASDKey() const
{
	if (WASDOverlay)
	{
		WASDOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USMTutorialKeyInfo::InternalHideEKey() const
{
	if (EOverlay)
	{
		EOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}
