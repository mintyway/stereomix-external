// Copyright Studio Surround. All Rights Reserved.


#include "SMTransitionManager.h"

#include "SMTransitionScreen.h"
#include "SMTransitionSettings.h"
#include "Framework/Application/IInputProcessor.h"
#include "Player/SMPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixTransition, Log, All);

// Input processor to throw in when loading screen is shown
// This will capture any inputs, so active menus under the loading screen will not interact
class FTransitionScreenInputPreProcessor final : public IInputProcessor
{
public:
	FTransitionScreenInputPreProcessor()
	{
	}

	virtual ~FTransitionScreenInputPreProcessor() override
	{
	}

	static bool CanEatInput()
	{
		return !GIsEditor;
	}

	//~IInputProcess interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return CanEatInput(); }
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return CanEatInput(); }
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return CanEatInput(); }
	//~End of IInputProcess interface
};

USMTransitionManager* USMTransitionManager::Get(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetSubsystem<USMTransitionManager>();
	}
	return nullptr;
}

bool USMTransitionManager::IsTransOut(const UObject* InWorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(InWorldContextObject);
	const USMTransitionManager* TransitionManager = World->GetSubsystem<USMTransitionManager>();
	return TransitionManager->ActiveTransitionScreen != nullptr;
}

void USMTransitionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	if (GetWorld()->GetGameInstance()->IsDedicatedServerInstance())
	{
		return;
	}

	LoadingScreenManager = InWorld.GetGameInstance()->GetSubsystem<ULoadingScreenManager>();
	if (LoadingScreenManager)
	{
		LoadingScreenManager->OnLoadingScreenVisibilityChangedDelegate().AddUObject(this, &ThisClass::OnLoadingScreenVisibilityChanged);
	}

	if (ASMPlayerController* StereoMixPC = Cast<ASMPlayerController>(InWorld.GetFirstPlayerController()))
	{
		TSoftClassPtr<USMTransitionScreen> TransitionScreenClassPtr = StereoMixPC->GetTransitionScreenClass();
		PrepareTransitionScreen(TransitionScreenClassPtr);

		StereoMixPC->WhenReadyToPlay(FOnActorReadyToPlay::FDelegate::CreateUObject(this, &ThisClass::HandleLocalPlayerReadyToPlay));
	}
}

void USMTransitionManager::Deinitialize()
{
	Super::Deinitialize();

	// Transition이 완료되지 않은 상태에서 레벨이 전환되는 경우가 있을 수 있으므로 Input Blocking을 해제함
	StopBlockingInput();
}

bool USMTransitionManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (IsRunningDedicatedServer())
	{
		return false;
	}
	UWorld* World = Cast<UWorld>(Outer);
	if (World && !World->IsGameWorld())
	{
		return false;
	}
	return Super::ShouldCreateSubsystem(Outer);
}

bool USMTransitionManager::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

bool USMTransitionManager::PrepareTransitionScreen(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass)
{
	if (!GetWorld()->GetGameViewport())
	{
		UE_LOG(LogStereoMixTransition, Error, TEXT("Failed to add transition screen widget to viewport: GameViewport is not set."));
		return false;
	}

	const USMTransitionSettings* TransitionSettings = GetDefault<USMTransitionSettings>();

	// 대상 트랜지션이 없는 경우 기본 트랜지션 스크린 클래스를 사용함
	TSubclassOf<USMTransitionScreen> TargetTransitionScreenClass = TransitionScreenClass.LoadSynchronous();
	if (!TargetTransitionScreenClass)
	{
		// 1. Local PlayerController에서 가져옴
		if (UWorld* World = GetWorld())
		{
			if (ASMPlayerController* StereoMixPC = Cast<ASMPlayerController>(World->GetFirstPlayerController()))
			{
				TargetTransitionScreenClass = StereoMixPC->GetTransitionScreenClass().LoadSynchronous();
			}
		}

		// 2. ini Config에서 가져옴
		if (!TargetTransitionScreenClass)
		{
			TargetTransitionScreenClass = TransitionSettings->DefaultTransitionScreenClass.LoadSynchronous();
		}

		if (!TargetTransitionScreenClass)
		{
			UE_LOG(LogStereoMixTransition, Error, TEXT("Failed to load default transition screen class."));
			return false;
		}
	}

	if (!TargetTransitionScreenClass)
	{
		UE_LOG(LogStereoMixTransition, Error, TEXT("Failed to load transition screen class."));
		return false;
	}

	if (ActiveTransitionScreen && ActiveTransitionScreen->GetClass() != TargetTransitionScreenClass)
	{
		UE_LOG(LogStereoMixTransition, Verbose, TEXT("Transition screen already created but different class. Replacing with new transition screen."));
		ActiveTransitionScreen->RemoveFromParent();
		ActiveTransitionScreen = nullptr;
	}

	if (!ActiveTransitionScreen)
	{
		// Create new transition screen widget
		UE_LOG(LogStereoMixTransition, Verbose, TEXT("Creating new transition screen widget."));
		ActiveTransitionScreen = CreateWidget<USMTransitionScreen>(GetWorld(), TargetTransitionScreenClass);
		if (!ActiveTransitionScreen)
		{
			UE_LOG(LogStereoMixTransition, Error, TEXT("Failed to create transition screen widget."));
			return false;
		}
		ActiveTransitionScreen->GetTransitionCompletedDelegate().BindUObject(this, &ThisClass::HandleTransitionCompleted);
	}

	if (!ActiveTransitionScreen->IsInViewport())
	{
		const int32 ZOrder = TransitionSettings->TransitionScreenZOrder;
		ActiveTransitionScreen->AddToViewport(ZOrder);
	}

	return true;
}

void USMTransitionManager::DoTransitionInternal(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass, ESMTransitionType TransitionType, FSMTransitionDelegate&& InCompletedDelegate)
{
	if (GetWorld()->GetGameInstance()->IsDedicatedServerInstance())
	{
		return;
	}

	if (!ensure(LoadingScreenManager))
	{
		return;
	}

	if (ActiveTransition.IsSet())
	{
		UE_LOG(LogStereoMixTransition, Log, TEXT("Already showing a transition screen, ignoring this request: %s"), *UEnum::GetValueAsString(ActiveTransition.GetTransitionType()));
		(void)InCompletedDelegate.ExecuteIfBound();
		return;
	}

	if (TransitionType == ESMTransitionType::In && LoadingScreenManager->GetLoadingScreenDisplayStatus())
	{
		UE_LOG(LogStereoMixTransition, Warning, TEXT("Cannot show transition screen while loading screen is visible."));
		(void)InCompletedDelegate.ExecuteIfBound();
		return;
	}

	if (!PrepareTransitionScreen(TransitionScreenClass))
	{
		(void)InCompletedDelegate.ExecuteIfBound();
		return;
	}

	FSMTransitionProcess NewTransition(TransitionType, InCompletedDelegate);
	ActiveTransition = MoveTemp(NewTransition);

	StartTransition();
	// 바로 StartTransition을 호출하지 않더라도 StartBlockingInput을 호출하여 입력을 차단함 차단된 입력은 Transition이 완료되면 해제됨
	// StartBlockingInput();
}

void USMTransitionManager::HandleLocalPlayerReadyToPlay(IActorInitStateInterface* ActorToReady)
{
	StartTransition();
}

void USMTransitionManager::OnLoadingScreenVisibilityChanged(bool bVisibleLoadingScreen)
{
	// Transition Out
	if (GetWorld())
	{
		if (!bVisibleLoadingScreen)
		{
			if (ActiveTransition.IsSet() && ActiveTransition.GetTransitionType() == ESMTransitionType::Out)
			{
				StartTransition(true);
			}
		}
	}
}

void USMTransitionManager::StartTransition(bool bForceStart)
{
	if (!ActiveTransition.IsSet())
	{
		return;
	}
	if (!ActiveTransitionScreen || ActiveTransitionScreen->IsTransitioning())
	{
		return;
	}

	if (!bForceStart)
	{
		if (ASMPlayerController* StereoMixPC = Cast<ASMPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (!StereoMixPC->IsReadyToPlay())
			{
				return;
			}
		}
		if (LoadingScreenManager->GetLoadingScreenDisplayStatus())
		{
			return;
		}
	}

	ActiveTransitionScreen->StartTransition(ActiveTransition.GetTransitionType());
}

void USMTransitionManager::HandleTransitionCompleted()
{
	if (ActiveTransition.IsSet())
	{
		ESMTransitionType TransitionType = ActiveTransition.GetTransitionType();
		// TransitionOut인 경우에만 뷰포트에서 위젯을 제거함
		if (TransitionType == ESMTransitionType::Out)
		{
			ActiveTransitionScreen->RemoveFromParent();
			// StopBlockingInput();
		}
		(void)ActiveTransition.GetCompletedDelegate().ExecuteIfBound();
		ActiveTransition.Reset();

		OnTransitionCompletedEvent.Broadcast(TransitionType);
		K2_OnTransitionCompletedEvent.Broadcast(TransitionType);
	}
}

void USMTransitionManager::StartBlockingInput()
{
	if (!InputPreProcessor.IsValid())
	{
		InputPreProcessor = MakeShareable<FTransitionScreenInputPreProcessor>(new FTransitionScreenInputPreProcessor());
		FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 1);
	}
}

void USMTransitionManager::StopBlockingInput()
{
	if (InputPreProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
		InputPreProcessor.Reset();
	}
}
