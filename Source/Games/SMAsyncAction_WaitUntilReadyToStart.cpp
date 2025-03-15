// Copyright Studio Surround. All Rights Reserved.


#include "SMAsyncAction_WaitUntilReadyToStart.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMAsyncAction_WaitUntilReadyToStart)

USMAsyncAction_WaitUntilReadyToStart* USMAsyncAction_WaitUntilReadyToStart::WaitUntilReadyToStart(UObject* InWorldContextObject)
{
	USMAsyncAction_WaitUntilReadyToStart* Action = NewObject<USMAsyncAction_WaitUntilReadyToStart>();
	Action->WorldContextObject = InWorldContextObject;
	return Action;
}

void USMAsyncAction_WaitUntilReadyToStart::Activate()
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull))
	{
		if (ASMGameState* GameState = World->GetGameState<ASMGameState>())
		{
			CheckGameStateReadyToStart(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::CheckGameStateReadyToStart);
		}
	}
	else
	{
		SetReadyToDestroy();
	}
}

void USMAsyncAction_WaitUntilReadyToStart::CheckGameStateReadyToStart(AGameStateBase* GameState)
{
	if (ASMGameState* StereoMixGameState = Cast<ASMGameState>(GameState))
	{
		if (StereoMixGameState->IsReadyToStart())
		{
			HandleGameReadyToStart();
		}
		else
		{
			StereoMixGameState->OnGameReadyToStartEvent.AddUObject(this, &ThisClass::HandleGameReadyToStart);
		}
	}
	else
	{
		SetReadyToDestroy();
	}
}

void USMAsyncAction_WaitUntilReadyToStart::HandleGameReadyToStart()
{
	OnGameReadyToStart.Broadcast();
	OnGameReadyToStart.Clear();
	SetReadyToDestroy();
}

void USMAsyncAction_WaitUntilReadyToStart::HandlePlayerReadyToPlay()
{
}
