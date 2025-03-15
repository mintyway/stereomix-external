// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "Data/SMCharacterType.h"
#include "SMAssetManagerFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAssetFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static FPrimaryAssetId GetCharacterDataAssetID(ESMCharacterType CharacterType);
};
