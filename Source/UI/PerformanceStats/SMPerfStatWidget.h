// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Performance/SMPerformanceStatTypes.h"
#include "SMPerfStatWidget.generated.h"

class USMPerformanceStatSubsystem;

/**
 * 개별 성능 통계를 표시하는 위젯
 */
UCLASS(Abstract, DisplayName = "StereoMix Performance Stat Widget")
class STEREOMIX_API USMPerfStatWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	ESMDisplayablePerformanceStat GetStatToDisplay() const
	{
		return StatToDisplay;
	}

	UFUNCTION(BlueprintPure)
	double FetchStatValue();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	ESMDisplayablePerformanceStat StatToDisplay;

	UPROPERTY(Transient)
	TObjectPtr<USMPerformanceStatSubsystem> CachedStatSubsystem;
};
