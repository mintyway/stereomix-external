// Copyright Studio Surround. All Rights Reserved.


#include "SMPageEntryInterface.h"

#include "Blueprint/UserWidget.h"


void ISMPageEntryInterface::SetItemObject(UUserWidget* PageEntryWidget, USMPageEntryObject* InItem)
{
	if (ensure(PageEntryWidget))
	{
		// Native implementation과 Blueprint implementation을 모두 확인하고 호출
		if (ISMPageEntryInterface* NativeImplementation = Cast<ISMPageEntryInterface>(PageEntryWidget))
		{
			NativeImplementation->NativeOnItemObjectSet(InItem);
		}
		else if (PageEntryWidget->Implements<USMPageEntryInterface>())
		{
			Execute_OnItemObjectSet(PageEntryWidget, InItem);
		}
	}
}

// Add default functionality here for any ISMPageEntryInterface functions that are not pure virtual.
void ISMPageEntryInterface::NativeOnItemObjectSet(USMPageEntryObject* InItem)
{
	Execute_OnItemObjectSet(Cast<UObject>(this), InItem);
}
