// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPerfStatWidget.h"
#include "SMPerfSingleGraphStatWidget.generated.h"


class UCommonTextBlock;

UCLASS(Abstract, DisplayName = "StereoMix Single Graph Stat Widget")
class STEREOMIX_API USMPerfSingleGraphStatWidget : public USMPerfStatWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	static FText FormatValue(float InValue, const FText& InFormat, int32 InFractionalDigits, float InScaleFactor);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> LabelTextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ValueTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	// 포맷 스트링에 {Value}를 포함하고 있어야 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	FText ValueFormat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	int32 FractionalDigits = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	float ScaleFactor = 1.0f;
};
