// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayPopWidget.h"

#include "SMGameplayPopDescriptors.h"
#include "SMGameplayTags.h"
#include "Animation/UMGSequencePlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameplayPopWidget)


void USMGameplayPopWidget::ActivateGameplayPop(const USMGameplayPopDescriptor* InDescriptor)
{
	if (!ensure(InDescriptor))
	{
		return;
	}
	CurrentAnimation = nullptr;
	BP_OnActivatePop(InDescriptor);
}

void USMGameplayPopWidget::FinishPlay()
{
	BP_OnDeactivatePop();
	(void)OnFinishPlay.ExecuteIfBound();
}

void USMGameplayPopWidget::PlayPopAnimation(UWidgetAnimation* Animation)
{
	CurrentAnimation = Animation;
	QueuePlayAnimationForward(Animation);
}

void USMGameplayPopWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == CurrentAnimation)
	{
		FinishPlay();
	}
}
