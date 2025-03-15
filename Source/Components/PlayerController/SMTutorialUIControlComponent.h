// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMTutorialUIControlComponent.generated.h"

class USMTutorialKeyInfo;
DECLARE_DELEGATE(FOnWidgetTransitionEnded);

class USMTutorialSuccess;
class USMTutorialMission;
class USMTutorialGuide;
class USMTutorialHUD;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialUIControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTutorialUIControlComponent();

	virtual void Activate(bool bReset) override;

	USMTutorialHUD* GetTutorialHUD() const { return TutorialHUD; }

	USMTutorialGuide* GetTutorialGuide() const;

	USMTutorialSuccess* GetTutorialSuccess() const;

	USMTutorialMission* GetTutorialMission() const;

	USMTutorialKeyInfo* GetTutorialKeyInfo() const;

	void SetGuideText(const FText& InText);

	void SetMissionText(const FText& InText);

	void PlayShowGuideAnimation();

	void TransitionToSuccess();

	void TransitionToGuide();

	void TransitionAndSetText(const FText& InGuideText, const FText& InMissionText);

	void HideAllKeyInfo();

	void ShowLeftClick();

	void HideLeftClick();

	void ShowRightClick();

	void HideRightClick();

	void ShowRightLeftClick(bool bUseDimLeftClick = false, bool bUseDimRightClick = false);

	void HideRightLeftClick();

	void ShowWASDKey();

	void HideWASDKey();

	void ShowEKey();

	void HideEKey();

	FOnWidgetTransitionEnded OnTransitionAndSetTextEnded;

protected:
	void InternalTransitionToSuccess();

	UFUNCTION()
	void OnHideMissionAnimationEnded();

	UFUNCTION()
	void OnHideGuideAnimationEnded();

	UFUNCTION()
	void OnShowSuccessAnimationEnded();

	void InternalTransitionToGuide();

	UFUNCTION()
	void OnHideSuccessAnimationEnded();

	UFUNCTION()
	void OnShowGuideAnimationEnded();

	UFUNCTION()
	void OnShowMissionAnimationEnded();

	void InternalTransitionAndSetText(const FText& InGuideText, const FText& InMissionText);

	void OnTransitionToSuccessEndedCallback();

	void SetupGuideTransition();

	void OnTransitionAndSetTextEndedCallback();

	/** 목표 달성을 보여주는 시간입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float CompletionDisplayTime = 1.0f;

	FOnWidgetTransitionEnded OnTransitionToSuccessEnded;

	FOnWidgetTransitionEnded OnTransitionToGuideEnded;

	FText GuideText;

	FText MissionText;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<USMTutorialHUD> TutorialHUDClass;

	UPROPERTY()
	TObjectPtr<USMTutorialHUD> TutorialHUD;
};
