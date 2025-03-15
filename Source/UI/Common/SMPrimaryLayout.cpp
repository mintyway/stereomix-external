// Copyright Studio Surround. All Rights Reserved.


#include "SMPrimaryLayout.h"

TOptional<FUIInputConfig> USMPrimaryLayout::GetDesiredInputConfig() const
{
	GetOwningPlayer()->CurrentMouseCursor = DefaultMouseCursor;
	return Super::GetDesiredInputConfig();
}

void USMPrimaryLayout::NativeOnInitialized()
{
	SetCursor(DefaultMouseCursor);
	Super::NativeOnInitialized();
}

void USMPrimaryLayout::NativeOnActivated()
{
	Super::NativeOnActivated();
}
