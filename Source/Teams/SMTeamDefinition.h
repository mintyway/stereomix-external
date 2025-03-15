// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMTeamDefinition.generated.h"

enum class ESMTeam : uint8;

/**
 * StereoMix Team Definition
 */
UCLASS(BlueprintType, DisplayName = "StereoMix Team Definition")
class STEREOMIX_API USMTeamDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("Team")), GetFName());
	}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Team")
	ESMTeam Team;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Team")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Team")
	TSoftObjectPtr<UTexture2D> CoverImage;
};
