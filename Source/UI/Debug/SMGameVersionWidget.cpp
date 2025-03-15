// Copyright Studio Surround. All Rights Reserved.


#include "SMGameVersionWidget.h"

#include "Components/TextBlock.h"
#include "System/SMGameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameVersionWidget)

void USMGameVersionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const FString BuildVersion = USMGameplayStatics::GetBuildVersion();
	VersionTextBlock->SetText(FText::FromString(BuildVersion));
}
