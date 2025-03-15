// Copyright Studio Surround. All Rights Reserved.


#include "SMBoundActionBar.h"

UUserWidget* USMBoundActionBar::CreateActionButton(const FUIActionBindingHandle& BindingHandle)
{
	UUserWidget* NewButton = Super::CreateActionButton(BindingHandle);
	if (NewButton)
	{
	}
	
	return NewButton;
}
