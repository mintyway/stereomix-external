// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "SMGuideKeyData.generated.h"

/**
 * StereoMix Guide Keys Data Asset
 */
UCLASS(BlueprintType, DisplayName = "Guide Key Data")
class STEREOMIX_API USMGuideKeyData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Guide Keys")
	TArray<TObjectPtr<UInputAction>> ActionsToDisplay;
};
