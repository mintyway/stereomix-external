// Copyright Studio Surround. All Rights Reserved.


#include "SMEmoticonWidget.h"


void USMEmoticonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);
}

void USMEmoticonWidget::PlayHiEmoticon()
{
	PreparePlayEmoticonAnimation();
	PlayAnimationForward(HiAnimation);
}

void USMEmoticonWidget::PlayGoodEmoticon()
{
	PreparePlayEmoticonAnimation();
	PlayAnimationForward(GoodAnimation);
}

void USMEmoticonWidget::PlaySadEmoticon()
{
	PreparePlayEmoticonAnimation();
	PlayAnimationForward(SadAnimation);
}

void USMEmoticonWidget::PreparePlayEmoticonAnimation()
{
	StopAllAnimations();
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
