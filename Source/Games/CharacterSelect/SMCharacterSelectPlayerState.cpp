// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerState.h"

#include "SMCharacterSelectLog.h"
#include "Net/UnrealNetwork.h"
#include "SMCharacterSelectState.h"
#include "Components/Common/SMTeamComponent.h"
#include "Utilities/SMLog.h"


ASMCharacterSelectPlayerState::ASMCharacterSelectPlayerState()
{
	CurrentState = ECharacterSelectPlayerStateType::Loading;
	FocusCharacterType = ESMCharacterType::None;
}

void ASMCharacterSelectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentState)
	DOREPLIFETIME(ThisClass, FocusCharacterType)
}

void ASMCharacterSelectPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMCharacterSelectPlayerState::BeginPlay()
{
	Super::BeginPlay();
	CharacterSelectState = GetWorld()->GetGameState<ASMCharacterSelectState>();
}

void ASMCharacterSelectPlayerState::ChangeFocusCharacterType_Implementation(ESMCharacterType NewCharacterType)
{
	FocusCharacterType = NewCharacterType;
	OnRep_FocusCharacterType();
}

void ASMCharacterSelectPlayerState::ChangeCharacterType_Implementation(ESMCharacterType NewCharacterType)
{
	if (const UWorld* World = GetWorld(); World && NewCharacterType != ESMCharacterType::None)
	{
		if (const ASMCharacterSelectState* GameState = World->GetGameState<ASMCharacterSelectState>(); GameState)
		{
			for (TArray<APlayerState*> PlayerArray = GameState->PlayerArray; APlayerState* PlayerState : PlayerArray)
			{
				if (const ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(PlayerState))
				{
					if (PlayerState == this)
					{
						continue;
					}

					if (SMPlayerState->GetTeam() == GetTeam() && SMPlayerState->GetCharacterType() == NewCharacterType)
					{
						SetCharacterType(ESMCharacterType::None);
						ResponseChangeCharacter(false, ESMCharacterType::None);
						NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Warning, TEXT("Player %s failed to change character type: %s is already selected by %s"), *GetPlayerName(), *UEnum::GetValueAsString(NewCharacterType), *SMPlayerState->GetPlayerName())
						return;
					}
				}
			}
		}
	}

	SetCharacterType(NewCharacterType);
	ResponseChangeCharacter(true, NewCharacterType);
	NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Log, TEXT("Character changed to %s"), *UEnum::GetValueAsString(NewCharacterType));
}

void ASMCharacterSelectPlayerState::ResponseChangeCharacter_Implementation(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (OnCharacterChangeResponse.IsBound())
	{
		OnCharacterChangeResponse.Broadcast(bSuccess, NewCharacterType);
	}
}

bool ASMCharacterSelectPlayerState::CanChangeCharacterType(const ESMCharacterType NewCharacterType) const
{
	return CharacterSelectState->GetCurrentState() == ECharacterSelectionStateType::Select;
}

void ASMCharacterSelectPlayerState::SetPredictFocusCharacterType_Implementation(ESMCharacterType NewFocusCharacterType)
{
	FocusCharacterType = NewFocusCharacterType;
}

void ASMCharacterSelectPlayerState::SetCurrentState_Implementation(const ECharacterSelectPlayerStateType NewState)
{
	CurrentState = NewState;
	if (OnCurrentStateChanged.IsBound())
	{
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
}

void ASMCharacterSelectPlayerState::OnRep_CurrentState()
{
	if (OnCurrentStateChanged.IsBound())
	{
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
}

void ASMCharacterSelectPlayerState::OnRep_FocusCharacterType()
{
	if (const UWorld* World = GetWorld())
	{
		if (ASMCharacterSelectState* GameStateNotify = Cast<ASMCharacterSelectState>(World->GetGameState()))
		{
			GameStateNotify->NotifyPlayerCharacterFocusChanged(this, FocusCharacterType);
		}
	}
}
