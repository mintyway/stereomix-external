// Copyright Studio Surround. All Rights Reserved.


#include "SMCommonBlueprintFunctionLibrary.h"

bool USMCommonBlueprintFunctionLibrary::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}
