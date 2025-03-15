﻿// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscrete.h"
#include "SMSettingValueDiscrete_Resolution.generated.h"

namespace EWindowMode
{
	enum Type : int;
}

struct FScreenResolutionRHI;

UCLASS(MinimalAPI)
class USMSettingValueDiscrete_Resolution : public UGameSettingValueDiscrete
{
	GENERATED_BODY()

public:
	USMSettingValueDiscrete_Resolution();

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	virtual int32 GetDiscreteOptionIndex() const override;
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	virtual void OnInitialized() override;
	virtual void OnDependencyChanged() override;

	void InitializeResolutions();
	bool ShouldAllowFullScreenResolution(const FScreenResolutionRHI& SrcScreenRes, int32 FilterThreshold) const;
	static void GetStandardWindowResolutions(const FIntPoint& MinResolution, const FIntPoint& MaxResolution, float MinAspectRatio, TArray<FIntPoint>& OutResolutions);
	void SelectAppropriateResolutions();
	int32 FindIndexOfDisplayResolution(const FIntPoint& InPoint) const;
	int32 FindIndexOfDisplayResolutionForceValid(const FIntPoint& InPoint) const;
	int32 FindClosestResolutionIndex(const FIntPoint& Resolution) const;

	TOptional<EWindowMode::Type> LastWindowMode;

	struct FScreenResolutionEntry
	{
		uint32 Width = 0;
		uint32 Height = 0;
		uint32 RefreshRate = 0;
		FText OverrideText;

		FIntPoint GetResolution() const { return FIntPoint(Width, Height); }
		FText GetDisplayText() const;
	};

	/** An array of strings the map to resolutions, populated based on the window mode */
	TArray<TSharedPtr<FScreenResolutionEntry>> Resolutions;

	/** An array of strings the map to fullscreen resolutions */
	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionsFullscreen;

	/** An array of strings the map to windowed fullscreen resolutions */
	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionsWindowedFullscreen;

	/** An array of strings the map to windowed resolutions */
	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionsWindowed;
};
