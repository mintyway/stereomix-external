// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscrete.h"
#include "SMSettingValueDiscrete_Language.generated.h"


UCLASS(MinimalAPI)
class USMSettingValueDiscrete_Language : public UGameSettingValueDiscrete
{
	GENERATED_BODY()

public:
	USMSettingValueDiscrete_Language();

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	virtual int32 GetDiscreteOptionIndex() const override;
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	virtual void OnInitialized() override;
	virtual void OnApply() override;

	TArray<FString> AvailableCultureNames;
};
