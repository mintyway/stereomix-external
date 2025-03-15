// Copyright Studio Surround. All Rights Reserved.


#include "SMAsyncAction_WaitUntilRoundState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMAsyncAction_WaitUntilRoundState)

USMAsyncAction_WaitUntilRoundState* USMAsyncAction_WaitUntilRoundState::WaitUntilRoundState(UObject* InWorldContextObject, ESMRoundState RoundState)
{
	USMAsyncAction_WaitUntilRoundState* Action = NewObject<USMAsyncAction_WaitUntilRoundState>();
	Action->WorldContextObject = InWorldContextObject;
	Action->TargetRoundState = RoundState;
	return Action;
}

void USMAsyncAction_WaitUntilRoundState::Activate()
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			HandleGameStateSet(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::HandleGameStateSet);
		}
	}
	else
	{
		SetReadyToDestroy();
	}
}

void USMAsyncAction_WaitUntilRoundState::HandleGameStateSet(AGameStateBase* GameState)
{
	if (USMRoundComponent* RoundComponent = GameState->GetComponentByClass<USMRoundComponent>())
	{
		if (RoundComponent->GetRoundState() >= TargetRoundState)
		{
			HandleRoundStateUpdated(ESMRoundState::None, RoundComponent->GetRoundState());
		}
		else
		{
			RoundComponent->OnRoundStateUpdatedEvent.AddUObject(this, &ThisClass::HandleRoundStateUpdated);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameState does not have USMRoundComponent."));
		SetReadyToDestroy();
	}
}

void USMAsyncAction_WaitUntilRoundState::HandleRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState)
{
	if (NewRoundState >= TargetRoundState)
	{
		OnRoundStateUpdated.Broadcast(PreviousRoundState, NewRoundState);
		OnRoundStateUpdated.Clear();
		SetReadyToDestroy();
	}
}
