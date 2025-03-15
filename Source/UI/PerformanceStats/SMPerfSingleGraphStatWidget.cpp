// Copyright Studio Surround. All Rights Reserved.


#include "SMPerfSingleGraphStatWidget.h"

#include "CommonTextBlock.h"

void USMPerfSingleGraphStatWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (LabelTextBlock)
	{
		LabelTextBlock->SetText(DisplayName);
	}
	if (ValueTextBlock)
	{
		ValueTextBlock->SetText(FormatValue(0.0f, ValueFormat, FractionalDigits, ScaleFactor));
	}
}

void USMPerfSingleGraphStatWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ValueTextBlock->SetText(FormatValue(FetchStatValue(), ValueFormat, FractionalDigits, ScaleFactor));
}

FText USMPerfSingleGraphStatWidget::FormatValue(float InValue, const FText& InFormat, int32 InFractionalDigits, float InScaleFactor)
{
	FText ValueText = FText::AsNumber(InValue * InScaleFactor,
		&FNumberFormattingOptions()
		 .SetMinimumFractionalDigits(InFractionalDigits)
		 .SetMaximumFractionalDigits(InFractionalDigits));
	return FText::FormatNamed(InFormat, TEXT("Value"), ValueText);
}
