// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMColorSet.generated.h"

enum class ESMTeam : uint8;


/**
 * StereoMix Color Set (Linear Color + GradientAtlasPosition)
 */
UCLASS(BlueprintType, DisplayName = "StereoMix Color Set", Category = "StereoMix|Colors")
class USMColorSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	FLinearColor GetColor() const { return Color; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	int32 GetGradientAtlasPosition() const { return GradientAtlasPosition; }

private:
	/** 팀 색상을 나타냅니다. */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	FLinearColor Color = FLinearColor::White;

	/** 팀 그라디언트를 나타냅니다. CA_StereoMixColor 아틀라스에 존재하는 커브의 포지션 값을 넣어야 합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	int32 GradientAtlasPosition = 0;
};

// BUG: UFUNCTION 매크로를 매크로와 함께 쓸 수 없는 버그
#define DEFINE_COLOR_SET_ACCESSOR(Name) \
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors") \
	const USMColorSet* Get##Name##ColorSet() const { return Name##ColorSet; } \
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors") \
	FLinearColor Get##Name##LinearColor() const { return Name##ColorSet ? Name##ColorSet->GetColor() : FLinearColor::White; } \
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors") \
	int32 Get##Name##GradientAtlasPosition() const { return Name##ColorSet ? Name##ColorSet->GetGradientAtlasPosition() : 0; }

/**
 * StereoMix Team Color Set
 */
UCLASS(BlueprintType, DisplayName = "StereoMix Team Color Set", Category = "StereoMix|Colors")
class STEREOMIX_API USMTeamColorSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	const USMColorSet* GetCommonColorSet() const { return CommonColorSet; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	FLinearColor GetCommonLinearColor() const { return CommonColorSet ? CommonColorSet->GetColor() : FLinearColor::White; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	int32 GetCommonGradientAtlasPosition() const { return CommonColorSet ? CommonColorSet->GetGradientAtlasPosition() : 0; };

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	const USMColorSet* GetLocalColorSet() const { return LocalColorSet; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	FLinearColor GetLocalLinearColor() const { return LocalColorSet ? LocalColorSet->GetColor() : FLinearColor::White; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	int32 GetLocalGradientAtlasPosition() const { return LocalColorSet ? LocalColorSet->GetGradientAtlasPosition() : 0; };

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	const USMColorSet* GetMyTeamColorSet() const { return MyTeamColorSet; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	FLinearColor GetMyTeamLinearColor() const { return MyTeamColorSet ? MyTeamColorSet->GetColor() : FLinearColor::White; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	int32 GetMyTeamGradientAtlasPosition() const { return MyTeamColorSet ? MyTeamColorSet->GetGradientAtlasPosition() : 0; };
	
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	const USMColorSet* GetOtherTeamColorSet() const { return OtherTeamColorSet; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	FLinearColor GetOtherTeamLinearColor() const { return OtherTeamColorSet ? OtherTeamColorSet->GetColor() : FLinearColor::White; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	int32 GetOtherTeamGradientAtlasPosition() const { return OtherTeamColorSet ? OtherTeamColorSet->GetGradientAtlasPosition() : 0; };

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	const USMColorSet* GetTeamColorSet(ESMTeam Team) const { return TeamColorSets.FindRef(Team); }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	FLinearColor GetTeamLinearColor(ESMTeam Team) const { return GetTeamColorSet(Team) ? GetTeamColorSet(Team)->GetColor() : FLinearColor::White; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Colors")
	int32 GetTeamGradientAtlasPosition(ESMTeam Team) const { return GetTeamColorSet(Team) ? GetTeamColorSet(Team)->GetGradientAtlasPosition() : 0; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	TObjectPtr<USMColorSet> CommonColorSet;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	TObjectPtr<USMColorSet> LocalColorSet;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	TObjectPtr<USMColorSet> MyTeamColorSet;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	TObjectPtr<USMColorSet> OtherTeamColorSet;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Colors")
	TMap<ESMTeam, TObjectPtr<USMColorSet>> TeamColorSets;
};
