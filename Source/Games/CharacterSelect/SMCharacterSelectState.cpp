// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectState.h"

#include "SMCharacterSelectLog.h"
#include "Components/Core/SMCountdownTimerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMPlayerState.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectState::ASMCharacterSelectState()
{
	CharacterSelectCountdownTime = 30;

	CurrentState = ECharacterSelectionStateType::Wait;
	CountdownTimer = CreateDefaultSubobject<USMCountdownTimerComponent>(TEXT("RoomCountdownTimer"));
}

void ASMCharacterSelectState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (HasAuthority())
	{
		// 캐릭터 선택 레벨에서 테스트하기 위해 임시로 팀 설정
		ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(PlayerState);
		if (SMPlayerState && SMPlayerState->GetTeam() == ESMTeam::None)
		{
			SMPlayerState->SetTeam(ESMTeam::FutureBass);
		}
	}
}

void ASMCharacterSelectState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}

bool ASMCharacterSelectState::ShouldShowLoadingScreen(FString& OutReason)
{
	if (CurrentState <= ECharacterSelectionStateType::Wait)
	{
		OutReason = TEXT("Character selection is not started yet.");
		return true;
	}
	return Super::ShouldShowLoadingScreen(OutReason);
}

void ASMCharacterSelectState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentState);
}

void ASMCharacterSelectState::SetCurrentState(ECharacterSelectionStateType NewState)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		CurrentState = NewState;
		NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Verbose, TEXT("Character selection state changed to %s"), *UEnum::GetValueAsString(NewState))
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
#endif
}

void ASMCharacterSelectState::NotifyPlayerCharacterFocusChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (OnPlayerCharacterFocusChanged.IsBound())
	{
		OnPlayerCharacterFocusChanged.Broadcast(Player, NewCharacter);
	}
}

void ASMCharacterSelectState::OnRep_CurrentState() const
{
	NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Verbose, TEXT("OnRep_CurrentState"));
	OnCurrentStateChanged.Broadcast(CurrentState);
}
