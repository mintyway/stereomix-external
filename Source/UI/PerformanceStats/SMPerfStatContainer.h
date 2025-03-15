// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Performance/SMPerformanceStatTypes.h"
#include "SMPerfStatContainer.generated.h"

/**
 * PerStatWidget 위젯들을 유저 설정에 따라 보여주거나 숨겨주는 패널입니다.
 */
UCLASS(Abstract, DisplayName = "StereoMix Performance Stat Container")
class STEREOMIX_API USMPerfStatContainer : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateVisibilityOfChildren();

protected:
	UPROPERTY(EditAnywhere, Category = Display)
	ESMStatDisplayMode StatDisplayModeFilter = ESMStatDisplayMode::TextOnly;
};
