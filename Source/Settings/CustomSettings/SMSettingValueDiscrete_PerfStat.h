// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscrete.h"
#include "SMSettingValueDiscrete_PerfStat.generated.h"


enum class ESMStatDisplayMode : uint8;
enum class ESMDisplayablePerformanceStat : uint8;

UCLASS(MinimalAPI)
class USMSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()

public:
	USMSettingValueDiscrete_PerfStat();

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	virtual int32 GetDiscreteOptionIndex() const override;
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual TArray<FText> GetDiscreteOptions() const override;

	void SetStat(ESMDisplayablePerformanceStat InStat);

protected:
	virtual void OnInitialized() override;

	void AddMode(FText&& Label, ESMStatDisplayMode Mode);

	TArray<FText> Options;
	TArray<ESMStatDisplayMode> DisplayModes;

	ESMDisplayablePerformanceStat StatToDisplay;
	ESMStatDisplayMode InitialMode;
};
