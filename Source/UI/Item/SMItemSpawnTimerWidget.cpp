// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemSpawnTimerWidget.h"

#include "Actors/Items/SMItemSpawner.h"
#include "CADKernel/Math/SlopeUtils.h"


void USMItemSpawnTimerWidget::SetItemSpawner(ASMItemSpawner* ItemSpawner)
{
	ItemSpawner->OnRemainingCooldownChanged.AddDynamic(this, &ThisClass::SetCooldownPercent);
}

void USMItemSpawnTimerWidget::SetCooldownPercent(float RemainTime, float Percent)
{
	SetValue(Percent);

	if (FMath::IsNearlyEqual(Percent, 1.0f, UE::CADKernel::Slope::Epsilon) || Percent >= 1.0f)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}

