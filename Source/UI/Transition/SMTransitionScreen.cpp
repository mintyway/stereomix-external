// Copyright Studio Surround. All Rights Reserved.


#include "SMTransitionScreen.h"

void USMTransitionScreen::StartTransition(ESMTransitionType TransitionType)
{
	bIsTransitioning = true;
	LastTransitionType = TransitionType;
	SetVisibility(ESlateVisibility::Visible);
	switch (TransitionType)
	{
		case ESMTransitionType::In:
			TransIn();
			break;
		case ESMTransitionType::Out:
			TransOut();
			break;
	}
}

void USMTransitionScreen::FinishTransition()
{
	bIsTransitioning = false;
	if (LastTransitionType == ESMTransitionType::Out)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	(void)TransitionCompletedDelegate.ExecuteIfBound();
}
