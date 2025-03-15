// Copyright Studio Surround. All Rights Reserved.


#include "AsyncAction_Transition.h"

#include "UI/Transition/SMTransitionManager.h"

UAsyncAction_Transition* UAsyncAction_Transition::TransIn(UObject* InWorldContextObject)
{
	UAsyncAction_Transition* Action = NewObject<UAsyncAction_Transition>();
	Action->WorldContextObject = InWorldContextObject;
	Action->TransitionScreenClass = nullptr;
	Action->TransitionType = ESMTransitionType::In;
	return Action;
}

UAsyncAction_Transition* UAsyncAction_Transition::TransOut(UObject* InWorldContextObject)
{
	UAsyncAction_Transition* Action = NewObject<UAsyncAction_Transition>();
	Action->WorldContextObject = InWorldContextObject;
	Action->TransitionScreenClass = nullptr;
	Action->TransitionType = ESMTransitionType::Out;
	return Action;
}

UAsyncAction_Transition* UAsyncAction_Transition::TransInUsingSpecificTransition(UObject* InWorldContextObject, TSoftClassPtr<USMTransitionScreen> InTransitionScreenClass)
{
	UAsyncAction_Transition* Action = NewObject<UAsyncAction_Transition>();
	Action->WorldContextObject = InWorldContextObject;
	Action->TransitionScreenClass = InTransitionScreenClass;
	Action->TransitionType = ESMTransitionType::In;
	return Action;
}

UAsyncAction_Transition* UAsyncAction_Transition::TransOutUsingSpecificTransition(UObject* InWorldContextObject, TSoftClassPtr<USMTransitionScreen> InTransitionScreenClass)
{
	UAsyncAction_Transition* Action = NewObject<UAsyncAction_Transition>();
	Action->WorldContextObject = InWorldContextObject;
	Action->TransitionScreenClass = InTransitionScreenClass;
	Action->TransitionType = ESMTransitionType::Out;
	return Action;
}

void UAsyncAction_Transition::Activate()
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert))
	{
		USMTransitionManager* TransitionManager = World->GetSubsystem<USMTransitionManager>();
		if (ensure(TransitionManager))
		{
			TransitionManager->DoTransition(TransitionScreenClass, TransitionType, this, &ThisClass::OnTransitionCompleted);
		}
		else
		{
			SetReadyToDestroy();
		}
	}
	else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_Transition::OnTransitionCompleted()
{
	if (OnCompleted.IsBound())
	{
		OnCompleted.Broadcast();
	}
	OnCompleted.Clear();
	SetReadyToDestroy();
}
