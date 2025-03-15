// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SMBuffDataAsset.generated.h"


UCLASS(BlueprintType, DisplayName = "StereoMix Buff Data Asset")
class STEREOMIX_API USMBuffDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("Buff")), GetFName());
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff", meta=(Categories = "Buff"))
	FGameplayTag BuffTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
	bool bShowInHUD = true;
};
