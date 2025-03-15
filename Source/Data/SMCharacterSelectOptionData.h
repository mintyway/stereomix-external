// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMCharacterType.h"

#include "SMCharacterSelectOptionData.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectOptionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Options")
	ESMCharacterType DefaultFocusedCharacterType;
};
