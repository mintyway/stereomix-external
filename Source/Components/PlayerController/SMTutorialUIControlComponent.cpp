// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialUIControlComponent.h"

#include "CommonTextBlock.h"
#include "PrimaryGameLayout.h"
#include "SMGameplayTags.h"
#include "Player/SMPlayerController.h"
#include "UI/Tutorial/SMTutorialGuide.h"
#include "UI/Tutorial/SMTutorialHUD.h"
#include "UI/Tutorial/SMTutorialKeyInfo.h"
#include "UI/Tutorial/SMTutorialMission.h"
#include "UI/Tutorial/SMTutorialSuccess.h"


USMTutorialUIControlComponent::USMTutorialUIControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USMTutorialUIControlComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (ASMPlayerController* OwnerController = GetOwner<ASMPlayerController>())
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(OwnerController))
		{
			RootLayout->PushWidgetToLayerStack<USMTutorialHUD>(StereoMix::Tags::UI_Layer_Game, TutorialHUDClass, [this](USMTutorialHUD& Widget) {
				TutorialHUD = Widget;
				if (TutorialHUD->TutorialSuccess)
				{
					TutorialHUD->TutorialSuccess->SetVisibility(ESlateVisibility::Hidden);
				}
			});
		}
	}
}

USMTutorialGuide* USMTutorialUIControlComponent::GetTutorialGuide() const
{
	return TutorialHUD ? TutorialHUD->TutorialGuide : nullptr;
}

USMTutorialSuccess* USMTutorialUIControlComponent::GetTutorialSuccess() const
{
	return TutorialHUD ? TutorialHUD->TutorialSuccess : nullptr;
}

USMTutorialMission* USMTutorialUIControlComponent::GetTutorialMission() const
{
	return TutorialHUD ? TutorialHUD->TutorialMission : nullptr;
}

USMTutorialKeyInfo* USMTutorialUIControlComponent::GetTutorialKeyInfo() const
{
	return TutorialHUD ? TutorialHUD->TutorialKeyInfo : nullptr;
}

void USMTutorialUIControlComponent::SetGuideText(const FText& InText)
{
	const USMTutorialGuide* TutorialGuide = GetTutorialGuide();
	if (UCommonTextBlock* GuideTextBlock = TutorialGuide ? TutorialGuide->GuideTextBlock : nullptr)
	{
		GuideTextBlock->SetText(InText);
	}
}

void USMTutorialUIControlComponent::SetMissionText(const FText& InText)
{
	const USMTutorialMission* TutorialMission = GetTutorialMission();
	if (UCommonTextBlock* DetailTextBlock = TutorialMission ? TutorialMission->DetailTextBlock : nullptr)
	{
		DetailTextBlock->SetText(InText);
	}
}

void USMTutorialUIControlComponent::PlayShowGuideAnimation()
{
	if (!TutorialHUD)
	{
		return;
	}

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->PlayAnimation(TutorialGuide->ShowGuideAnimation);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->QueuePlayAnimation(TutorialMission->ShowMissionAnimation);
	}
}

void USMTutorialUIControlComponent::TransitionToSuccess()
{
	InternalTransitionToSuccess();
}

void USMTutorialUIControlComponent::TransitionToGuide()
{
	InternalTransitionToGuide();
}

void USMTutorialUIControlComponent::TransitionAndSetText(const FText& InGuideText, const FText& InMissionText)
{
	InternalTransitionAndSetText(InGuideText, InMissionText);
}

void USMTutorialUIControlComponent::HideAllKeyInfo()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->HideAll();
	}
}

void USMTutorialUIControlComponent::ShowLeftClick()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->ShowLeftClick();
	}
}

void USMTutorialUIControlComponent::HideLeftClick()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->HideLeftClick();
	}
}

void USMTutorialUIControlComponent::HideRightClick()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->HideRightClick();
	}
}

void USMTutorialUIControlComponent::HideRightLeftClick()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->HideRightLeftClick();
	}
}

void USMTutorialUIControlComponent::HideWASDKey()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->HideWASDKey();
	}
}

void USMTutorialUIControlComponent::HideEKey()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->HideEKey();
	}
}

void USMTutorialUIControlComponent::ShowRightClick()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->ShowRightClick();
	}
}

void USMTutorialUIControlComponent::ShowRightLeftClick(bool bUseDimLeftClick, bool bUseDimRightClick)
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->ShowRightLeftClick(bUseDimLeftClick, bUseDimRightClick);
	}
}

void USMTutorialUIControlComponent::ShowWASDKey()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->ShowWASDKey();
	}
}

void USMTutorialUIControlComponent::ShowEKey()
{
	if (USMTutorialKeyInfo* TutorialKeyInfo = GetTutorialKeyInfo())
	{
		TutorialKeyInfo->ShowEKey();
	}
}

void USMTutorialUIControlComponent::InternalTransitionToSuccess()
{
	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		if (TutorialMission->HideMissionAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnHideMissionAnimationEnded);
			TutorialMission->UnbindAllFromAnimationFinished(TutorialMission->HideMissionAnimation);
			TutorialMission->BindToAnimationFinished(TutorialMission->HideMissionAnimation, EndDelegate);
			TutorialMission->PlayAnimation(TutorialMission->HideMissionAnimation);
		}
		else
		{
			OnHideMissionAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnHideMissionAnimationEnded()
{
	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		if (TutorialGuide->HideGuideAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnHideGuideAnimationEnded);
			TutorialGuide->UnbindAllFromAnimationFinished(TutorialGuide->HideGuideAnimation);
			TutorialGuide->BindToAnimationFinished(TutorialGuide->HideGuideAnimation, EndDelegate);
			TutorialGuide->PlayAnimation(TutorialGuide->HideGuideAnimation);
		}
		else
		{
			OnHideGuideAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnHideGuideAnimationEnded()
{
	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (TutorialSuccess->ShowSuccessAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnShowSuccessAnimationEnded);
			TutorialSuccess->UnbindAllFromAnimationFinished(TutorialSuccess->ShowSuccessAnimation);
			TutorialSuccess->BindToAnimationFinished(TutorialSuccess->ShowSuccessAnimation, EndDelegate);
			TutorialSuccess->PlayAnimation(TutorialSuccess->ShowSuccessAnimation);
		}
		else
		{
			OnShowSuccessAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnShowSuccessAnimationEnded()
{
	(void)OnTransitionToSuccessEnded.ExecuteIfBound();
}

void USMTutorialUIControlComponent::InternalTransitionToGuide()
{
	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		if (TutorialSuccess->HideSuccessAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnHideSuccessAnimationEnded);
			TutorialSuccess->UnbindAllFromAnimationFinished(TutorialSuccess->HideSuccessAnimation);
			TutorialSuccess->BindToAnimationFinished(TutorialSuccess->HideSuccessAnimation, EndDelegate);
			TutorialSuccess->PlayAnimation(TutorialSuccess->HideSuccessAnimation);
		}
		else
		{
			OnHideSuccessAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnHideSuccessAnimationEnded()
{
	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (TutorialGuide->ShowGuideAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnShowGuideAnimationEnded);
			TutorialGuide->UnbindAllFromAnimationFinished(TutorialGuide->ShowGuideAnimation);
			TutorialGuide->BindToAnimationFinished(TutorialGuide->ShowGuideAnimation, EndDelegate);
			TutorialGuide->PlayAnimation(TutorialGuide->ShowGuideAnimation);
		}
		else
		{
			OnShowGuideAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnShowGuideAnimationEnded()
{
	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (TutorialMission->ShowMissionAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnShowMissionAnimationEnded);
			TutorialMission->UnbindAllFromAnimationFinished(TutorialMission->ShowMissionAnimation);
			TutorialMission->BindToAnimationFinished(TutorialMission->ShowMissionAnimation, EndDelegate);
			TutorialMission->PlayAnimation(TutorialMission->ShowMissionAnimation);
		}
		else
		{
			OnShowMissionAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnShowMissionAnimationEnded()
{
	(void)OnTransitionToGuideEnded.ExecuteIfBound();
}

void USMTutorialUIControlComponent::InternalTransitionAndSetText(const FText& InGuideText, const FText& InMissionText)
{
	OnTransitionToSuccessEnded.Unbind();
	OnTransitionToGuideEnded.Unbind();

	GuideText = InGuideText;
	MissionText = InMissionText;

	OnTransitionToSuccessEnded.BindUObject(this, &ThisClass::OnTransitionToSuccessEndedCallback);
	TransitionToSuccess();
}

void USMTutorialUIControlComponent::OnTransitionToSuccessEndedCallback()
{
	OnTransitionToSuccessEnded.Unbind();
	SetGuideText(GuideText);
	SetMissionText(MissionText);

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SetupGuideTransition, CompletionDisplayTime, false);
	}
}

void USMTutorialUIControlComponent::SetupGuideTransition()
{
	OnTransitionToGuideEnded.BindUObject(this, &ThisClass::OnTransitionAndSetTextEndedCallback);
	TransitionToGuide();
}

void USMTutorialUIControlComponent::OnTransitionAndSetTextEndedCallback()
{
	OnTransitionToGuideEnded.Unbind();
	(void)OnTransitionAndSetTextEnded.ExecuteIfBound();
}
