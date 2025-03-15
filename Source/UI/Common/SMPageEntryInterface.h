// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMPageEntryInterface.generated.h"

class USMPageEntryObject;

UINTERFACE(BlueprintType, MinimalAPI, Category = Page)
class USMPageEntryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMPageEntryInterface
{
	GENERATED_BODY()

public:
	static void SetItemObject(UUserWidget* PageEntryWidget, USMPageEntryObject* InItem);

protected:
	virtual void NativeOnItemObjectSet(USMPageEntryObject* InItem);

	UFUNCTION(BlueprintImplementableEvent, Category = Page)
	void OnItemObjectSet(USMPageEntryObject* InItem);
};
