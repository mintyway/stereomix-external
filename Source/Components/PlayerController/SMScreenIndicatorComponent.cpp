// Copyright Studio Surround. All Rights Reserved.


#include "SMScreenIndicatorComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Player/SMPlayerController.h"
#include "UI/Game/SMScreenIndicatorWidget.h"
#include "Utilities/SMLog.h"


USMScreenIndicatorComponent::USMScreenIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USMScreenIndicatorComponent::AddScreenIndicator(AActor* TargetActor, const TSubclassOf<USMScreenIndicatorWidget>& ScreenIndicatorClass)
{
	ASMPlayerController* PlayerController = GetOwner<ASMPlayerController>();
	const bool bIsLocalController = PlayerController ? PlayerController->IsLocalPlayerController() : false;
	const auto ControlledPawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	if (!TargetActor || !bIsLocalController || !ControlledPawn)
	{
		return;
	}

	// 같은 팀인 경우 인디케이터를 생성하지 않습니다.
	if (USMTeamBlueprintLibrary::IsSameTeam(ControlledPawn, TargetActor))
	{
		return;
	}

	// 인디케이터를 생성하고 타겟을 지정합니다.
	if (USMScreenIndicatorWidget* ScreenIndicator = CreateWidget<USMScreenIndicatorWidget>(PlayerController, ScreenIndicatorClass))
	{
		ScreenIndicator->AddToViewport(-10);
		ScreenIndicator->SetTarget(TargetActor);
		ScreenIndicators.Add(TargetActor, ScreenIndicator);
	
		// 대상이 게임에서 나갔을 경우의 예외처리를 위한 바인드입니다.
		TargetActor->OnDestroyed.AddDynamic(this, &ThisClass::OnTargetDestroyed);
	}
}

void USMScreenIndicatorComponent::RemoveScreenIndicator(AActor* TargetActor)
{
	const ASMPlayerController* PlayerController = GetOwner<ASMPlayerController>();
	const bool bIsLocalController = PlayerController ? PlayerController->IsLocalPlayerController() : false;
	if (!TargetActor || !bIsLocalController)
	{
		return;
	}

	// 인디케이터를 제거합니다. 중복 제거되도 문제는 없습니다.
	const TObjectPtr<USMScreenIndicatorWidget>* ScreenIndicatorPtr = ScreenIndicators.Find(TargetActor);
	if (USMScreenIndicatorWidget* ScreenIndicator = ScreenIndicatorPtr ? *ScreenIndicatorPtr : nullptr)
	{
		ScreenIndicator->RemoveFromParent();
		ScreenIndicator->ConditionalBeginDestroy();
		ScreenIndicators.Remove(TargetActor);
	}

	// 대상이 게임에서 나갔을 경우의 예외처리로 사용된 이벤트를 제거합니다.
	TargetActor->OnDestroyed.RemoveAll(this);
}

void USMScreenIndicatorComponent::OnTargetDestroyed(AActor* DestroyedActor)
{
	// 대상이 게임에서 나가거나 어떤 이유로 유효하지 않게된 경우 인디케이터를 제거해줍니다.
	NET_LOG(GetOwner(), Warning, TEXT("유효하지 않은 타겟의 인디케이터 제거"));
	RemoveScreenIndicator(DestroyedActor);
}
