// Copyright Studio Surround. All Rights Reserved.


#include "SMLobbyComponent.h"

#include "GameFramework/PlayerState.h"
#include "SMLobbyLog.h"
#include "SMLobbySubsystem.h"
#include "SMLobbyPlayerComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Games/SMGameState.h"
#include "Match/SMMatchDefinition.h"
#include "Match/SMMatchSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMPlayerState.h"
#include "Settings/SMSettingsLocal.h"
#include "Utilities/SMLog.h"

USMLobbyComponent* USMLobbyComponent::Get(const AGameStateBase* InGameState)
{
	return InGameState ? InGameState->GetComponentByClass<USMLobbyComponent>() : nullptr;
}

USMLobbyComponent::USMLobbyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void USMLobbyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LobbyName, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsPrivateLobby, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LobbyOwner, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MaxPlayers, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentPlayers, SharedParams);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TeamPlayers_EDM, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TeamPlayers_FB, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, NonTeamPlayers, SharedParams);
}

void USMLobbyComponent::BeginPlay()
{
	Super::BeginPlay();

	USMSettingsLocal::Get()->SetShouldUseFrontendPerformanceSettings(true);
	
	if (HasAuthority())
	{
		StartLobby();
	}
}

void USMLobbyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	USMSettingsLocal::Get()->SetShouldUseFrontendPerformanceSettings(false);
}

void USMLobbyComponent::StartLobby()
{
	if (!HasAuthority())
	{
		return;
	}

	ASMGameState* GameState = GetGameState<ASMGameState>();
	if (!ensure(GameState))
	{
		return;
	}

	USMMatchSubsystem* MatchSubsystem = USMMatchSubsystem::Get(this);
	if (!MatchSubsystem)
	{
		UE_LOG(LogStereoMixLobby, Error, TEXT("MatchSubsystem is not found."));
		return;
	}

	USMLobbySubsystem* LobbySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMLobbySubsystem>();
	if (!LobbySubsystem)
	{
		UE_LOG(LogStereoMixLobby, Error, TEXT("LobbySubsystem is not found."));
		return;
	}

	const USMMatchDefinition* MatchDefinition = MatchSubsystem->GetMatchDefinition();
	if (!MatchDefinition)
	{
		UE_LOG(LogStereoMixLobby, Error, TEXT("MatchDefinition is not found."));
		return;
	}

	int32 MaxPlayersPerTeam = MatchDefinition->MaxPlayerCount / 2;

	TeamPlayers_FB.Empty(MaxPlayersPerTeam);
	TeamPlayers_EDM.Empty(MaxPlayersPerTeam);
	NonTeamPlayers.Empty(MatchDefinition->MaxPlayerCount);

	for (auto Player : GameState->PlayerArray)
	{
		if (ASMPlayerState* PlayerState = Cast<ASMPlayerState>(Player))
		{
			HandlePlayerAdded(PlayerState);
			PlayerState->GetTeamComponent()->OnTeamChanged.AddUObject(this, &ThisClass::HandlePlayerTeamChanged);
		}
	}

	GameState->OnPlayerAddedEvent.AddUObject(this, &ThisClass::HandlePlayerAdded);
	GameState->OnPlayerRemovedEvent.AddUObject(this, &ThisClass::HandlePlayerRemoved);


	// 기존 오너 플레이어 찾아서 캐싱
	// 방장 플레이어가 중간에 나가서 없으면 자동으로 다른 플레이어 중 하나를 방장으로 설정합니다.
	if (LobbySubsystem->HasOwner())
	{
		ChangeOwner(GetGameState<ASMGameState>()->GetPlayerStateFromUniqueNetId(LobbySubsystem->GetOwnerId()));
	}

	SetLobbyName(LobbySubsystem->GetLobbyName());
	SetIsPrivateLobby(LobbySubsystem->IsPrivateLobby());

	SetMaxPlayers(MatchDefinition->MaxPlayerCount);
	SetCurrentPlayers(GameState->PlayerArray.Num());

	bPendingNextMap = false;

	NET_LOG(GetOwner(), Log, TEXT("Lobby started."));
}

void USMLobbyComponent::SetMaxPlayers(int32 NewMaxPlayers)
{
	if (MaxPlayers != NewMaxPlayers)
	{
		MaxPlayers = NewMaxPlayers;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MaxPlayers, this);
	}
}

void USMLobbyComponent::SetCurrentPlayers(int32 NewCurrentPlayers)
{
	if (CurrentPlayers != NewCurrentPlayers)
	{
		CurrentPlayers = NewCurrentPlayers;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentPlayers, this);
	}
}

void USMLobbyComponent::AddPlayerToTeam(const ASMPlayerState* Player, ESMTeam Team)
{
	if (USMLobbyPlayerComponent* LobbyPlayer = USMLobbyPlayerComponent::Get(Player))
	{
		switch (Team)
		{
			case ESMTeam::None:
				NonTeamPlayers.AddUnique(LobbyPlayer);
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, NonTeamPlayers, this);
				break;
			case ESMTeam::EDM:
				TeamPlayers_EDM.AddUnique(LobbyPlayer);
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TeamPlayers_EDM, this);
				break;
			case ESMTeam::FutureBass:
				TeamPlayers_FB.AddUnique(LobbyPlayer);
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TeamPlayers_FB, this);
				break;
			case ESMTeam::Max:
				UE_LOG(LogStereoMixLobby, Error, TEXT("Invalid team type. (Player: %s(%s), %d)"),
					*Player->GetPlayerName(),
					*Player->GetUniqueId().ToDebugString(),
					(int32)Player->GetTeam());
				break;
		}
		UE_LOG(LogStereoMixLobby, Verbose, TEXT("Player %s(%s) added to team %s"),
			*Player->GetPlayerName(),
			*Player->GetUniqueId().ToDebugString(),
			*UEnum::GetValueAsString(Team));
	}

	BroadcastLobbyPlayersUpdatedOnNextTick();
}

void USMLobbyComponent::RemovePlayerFromTeam(const ASMPlayerState* Player, ESMTeam Team)
{
	if (USMLobbyPlayerComponent* LobbyPlayer = USMLobbyPlayerComponent::Get(Player))
	{
		switch (Team)
		{
			case ESMTeam::None:
				NonTeamPlayers.Remove(LobbyPlayer);
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, NonTeamPlayers, this);
				break;
			case ESMTeam::EDM:
				TeamPlayers_EDM.Remove(LobbyPlayer);
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TeamPlayers_EDM, this);
				break;
			case ESMTeam::FutureBass:
				TeamPlayers_FB.Remove(LobbyPlayer);
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TeamPlayers_FB, this);
				break;
			case ESMTeam::Max:
				UE_LOG(LogStereoMixLobby, Error, TEXT("Invalid team type. (Player: %s(%s), %d)"),
					*Player->GetPlayerName(),
					*Player->GetUniqueId().ToDebugString(),
					(int32)Player->GetTeam());
				break;
		}
		UE_LOG(LogStereoMixLobby, Verbose, TEXT("Player %s(%s) removed from team %s"),
			*Player->GetPlayerName(),
			*Player->GetUniqueId().ToDebugString(),
			*UEnum::GetValueAsString(Team));
	}

	BroadcastLobbyPlayersUpdatedOnNextTick();
}

void USMLobbyComponent::HandlePlayerAdded(ASMPlayerState* NewPlayer)
{
	AddPlayerToTeam(NewPlayer, NewPlayer->GetTeam());
	NewPlayer->GetTeamComponent()->OnTeamChanged.AddUObject(this, &ThisClass::HandlePlayerTeamChanged);
	SetCurrentPlayers(GetGameState<ASMGameState>()->PlayerArray.Num());
	BroadcastLobbyPlayersUpdatedOnNextTick();
}

void USMLobbyComponent::HandlePlayerRemoved(ASMPlayerState* RemovedPlayer)
{
	RemovePlayerFromTeam(RemovedPlayer, RemovedPlayer->GetTeam());
	SetCurrentPlayers(GetGameState<ASMGameState>()->PlayerArray.Num());
	BroadcastLobbyPlayersUpdatedOnNextTick();
}

void USMLobbyComponent::HandlePlayerTeamChanged(AActor* Actor, ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	if (ASMPlayerState* PlayerState = Cast<ASMPlayerState>(Actor))
	{
		RemovePlayerFromTeam(PlayerState, PreviousTeam);
		AddPlayerToTeam(PlayerState, NewTeam);
		BroadcastLobbyPlayersUpdatedOnNextTick();
	}
}

void USMLobbyComponent::BroadcastLobbyInfoUpdated()
{
	OnLobbyInfoUpdatedEvent.Broadcast();
	// UGameplayMessageSubsystem::Get(this).BroadcastMessage()
}

void USMLobbyComponent::BroadcastLobbyInfoUpdatedOnNextTick()
{
	if (!LobbyInfoBroadcastTimerHandle.IsValid())
	{
		LobbyInfoBroadcastTimerHandle = GetWorldTimerManager().SetTimerForNextTick([this] {
			BroadcastLobbyInfoUpdated();
			LobbyInfoBroadcastTimerHandle.Invalidate();
		});
	}
}

void USMLobbyComponent::BroadcastLobbyPlayersUpdated()
{
	OnLobbyPlayersUpdatedEvent.Broadcast();
	// UGameplayMessageSubsystem::Get(this).BroadcastMessage()
}

void USMLobbyComponent::BroadcastLobbyPlayersUpdatedOnNextTick()
{
	if (!LobbyPlayersBroadcastTimerHandle.IsValid())
	{
		LobbyPlayersBroadcastTimerHandle = GetWorldTimerManager().SetTimerForNextTick([this] {
			BroadcastLobbyPlayersUpdated();
			LobbyPlayersBroadcastTimerHandle.Invalidate();
		});
	}
}

void USMLobbyComponent::OnRep_LobbyInfo()
{
	BroadcastLobbyInfoUpdatedOnNextTick();
}

void USMLobbyComponent::OnRep_LobbyPlayers()
{
	BroadcastLobbyPlayersUpdatedOnNextTick();
}

void USMLobbyComponent::StartGame()
{
#if WITH_SERVER_CODE
	if (bPendingNextMap || !GetWorld() || !HasAuthority() || !CanStartGame())
	{
		return;
	}

	// None 팀 플레이어들을 랜덤하게 팀에 배정하되 각 팀 수의 밸런스가 맞아야 함
	TArray<USMLobbyPlayerComponent*> NoneTeamPlayersCopy = NonTeamPlayers;
	while (NoneTeamPlayersCopy.Num() > 0)
	{
		ESMTeam Team = FMath::RandBool() ? ESMTeam::EDM : ESMTeam::FutureBass;
		if (Team == ESMTeam::EDM && TeamPlayers_EDM.Num() >= TeamPlayers_FB.Num())
		{
			Team = ESMTeam::FutureBass;
		}
		else if (Team == ESMTeam::FutureBass && TeamPlayers_FB.Num() >= TeamPlayers_EDM.Num())
		{
			Team = ESMTeam::EDM;
		}

		USMLobbyPlayerComponent* Player = NoneTeamPlayersCopy.Pop();
		Player->ServerSetTeam(Team);

		ASMPlayerState* PlayerState = Player->GetPlayerState<ASMPlayerState>();
		UE_LOG(LogStereoMixLobby, Log, TEXT("Player %s(%s) is automatically assigned to team %s"),
			*PlayerState->GetPlayerName(),
			*PlayerState->GetUniqueId().ToDebugString(),
			*UEnum::GetValueAsString(Team));
	}

	UE_LOG(LogStereoMixLobby, Log, TEXT("[Final team assignments]"));
	for (USMLobbyPlayerComponent* Player : TeamPlayers_EDM)
	{
		ASMPlayerState* PlayerState = Player->GetPlayerState<ASMPlayerState>();
		UE_LOG(LogStereoMixLobby, Log, TEXT("EDM: %s(%s)"), *PlayerState->GetPlayerName(), *PlayerState->GetUniqueId().ToDebugString());
	}
	for (USMLobbyPlayerComponent* Player : TeamPlayers_FB)
	{
		ASMPlayerState* PlayerState = Player->GetPlayerState<ASMPlayerState>();
		UE_LOG(LogStereoMixLobby, Log, TEXT("FB: %s(%s)"), *PlayerState->GetPlayerName(), *PlayerState->GetUniqueId().ToDebugString());
	}

	if (USMMatchSubsystem* Match = USMMatchSubsystem::Get(this))
	{
		if (const USMMatchDefinition* MatchDefinition = Match->GetMatchDefinition())
		{
			FString NextUrl = MatchDefinition->GetCharacterSelectUrl();
			GetWorld()->ServerTravel(NextUrl, false);
			bPendingNextMap = true;

			UE_LOG(LogStereoMixLobby, Log, TEXT("Seamless traveling to %s"), *NextUrl);
		}
	}
#endif
}

bool USMLobbyComponent::CanStartGame() const
{
	ASMGameState* GameState = GetGameState<ASMGameState>();
	if (!GameState)
	{
		return false;
	}

	// 각 팀은 1명 이상의 플레이어가 있어야 합니다.
	if (TeamPlayers_FB.Num() < 1 || TeamPlayers_EDM.Num() < 1)
	{
		return false;
	}

	// 방장이 있어야 합니다.
	if (!LobbyOwner.IsValid())
	{
		return false;
	}

	// 방장이 팀을 선택한 경우에만 시작할 수 있습니다.
	if (LobbyOwner->GetTeam() == ESMTeam::None)
	{
		return false;
	}

	// 오너를 제외한 모든 플레이어가 레디 상태여야 합니다.
	for (TObjectPtr<APlayerState>& Player : GameState->PlayerArray)
	{
		if (USMLobbyPlayerComponent* LobbyPlayer = USMLobbyPlayerComponent::Get(Player.Get()))
		{
			if (!LobbyPlayer->IsOwner() && !LobbyPlayer->IsReady())
			{
				return false;
			}
		}
	}

	return true;
}

TArray<USMLobbyPlayerComponent*> USMLobbyComponent::GetTeamPlayers(ESMTeam Team) const
{
	switch (Team)
	{
		case ESMTeam::None:
			return NonTeamPlayers;
		case ESMTeam::EDM:
			return TeamPlayers_EDM;
		case ESMTeam::FutureBass:
			return TeamPlayers_FB;
		case ESMTeam::Max:
			UE_LOG(LogStereoMixLobby, Error, TEXT("Invalid team type. (Team: %d)"), (int32)Team);
			break;
	}
	return TArray<USMLobbyPlayerComponent*>();
}

USMLobbyPlayerComponent* USMLobbyComponent::GetLocalLobbyPlayer() const
{
	if (APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController())
	{
		// 혹시 서버에서 호출한 경우면 nullptr을 반환하도록 함
		if (LocalPlayerController->IsLocalController())
		{
			if (ASMPlayerState* LocalPlayerState = LocalPlayerController->GetPlayerState<ASMPlayerState>())
			{
				return USMLobbyPlayerComponent::Get(LocalPlayerState);
			}
		}
	}
	return nullptr;
}

USMLobbyPlayerComponent* USMLobbyComponent::GetLobbyOwner() const
{
	return LobbyOwner.IsValid() ? USMLobbyPlayerComponent::Get(LobbyOwner.Get()) : nullptr;
}

void USMLobbyComponent::ChangeOwner(APlayerState* NewOwner)
{
#if WITH_SERVER_CODE
	if (!HasAuthority())
	{
		return;
	}

	USMLobbySubsystem* LobbySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMLobbySubsystem>();
	if (!LobbySubsystem)
	{
		return;
	}

	// 플레이어가 올바르지 못하면 이외의 플레이어 중 하나를 선택합니다.
	if (!NewOwner)
	{
		for (const TObjectPtr<APlayerState>& Player : GetGameState<ASMGameState>()->PlayerArray)
		{
			if (Player && Player->IsA<ASMPlayerState>() && !Player->IsPendingKillPending())
			{
				NewOwner = Player.Get();
				break;
			}
		}
	}

	// 의도적으로 방장을 없애는 경우가 있기 때문에 NewOwner가 null인 경우도 처리합니다.
	if (NewOwner)
	{
		USMLobbyPlayerComponent* NewOwnerLobbyPlayer = USMLobbyPlayerComponent::Get(NewOwner);
		if (!ensure(NewOwnerLobbyPlayer))
		{
			UE_LOG(LogStereoMixLobby, Error, TEXT("Player %s does not have a LobbyPlayerComponent"), *NewOwner->GetPlayerName());
			return;
		}

		// 대상이 이미 방장인 경우 무시합니다.
		if (NewOwnerLobbyPlayer->IsOwner())
		{
			return;
		}
		NewOwnerLobbyPlayer->SetOwner(true);
		if (LobbyOwner.IsValid())
		{
			if (USMLobbyPlayerComponent* OldOwnerLobbyPlayer = USMLobbyPlayerComponent::Get(LobbyOwner.Get()))
			{
				OldOwnerLobbyPlayer->SetOwner(false);
			}
		}
		LobbyOwner = CastChecked<ASMPlayerState>(NewOwner);

		LobbySubsystem->SetOwner(NewOwner->GetUniqueId());
		UE_LOG(LogStereoMixLobby, Log, TEXT("Owner changed to %s"), *NewOwner->GetPlayerName());
	}
	else
	{
		LobbyOwner = nullptr;
		LobbySubsystem->SetOwner(FUniqueNetIdRepl());
		UE_LOG(LogStereoMixLobby, Log, TEXT("Owner changed to none. Now this lobby has no owner."));
	}
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, LobbyOwner, this);
	BroadcastLobbyInfoUpdatedOnNextTick();
#endif
}

bool USMLobbyComponent::TryChangeTeam(ASMPlayerState* Player, ESMTeam NewTeam)
{
	if (HasAuthority() && Player)
	{
		ESMTeam CurrentPlayerTeam = Player->GetTeam();
		if (CurrentPlayerTeam != NewTeam)
		{
			if (NewTeam == ESMTeam::None)
			{
				Player->SetTeam(NewTeam);
				return true;
			}
			if (NewTeam == ESMTeam::EDM && TeamPlayers_EDM.Num() < MaxPlayers / 2)
			{
				Player->SetTeam(NewTeam);
				return true;
			}
			if (NewTeam == ESMTeam::FutureBass && TeamPlayers_FB.Num() < MaxPlayers / 2)
			{
				Player->SetTeam(NewTeam);
				return true;
			}
		}
	}

	return false;
}

void USMLobbyComponent::SetLobbyName(const FString& NewLobbyName)
{
	if (LobbyName != NewLobbyName)
	{
		if (USMLobbySubsystem* Lobby = USMLobbySubsystem::Get(this))
		{
			Lobby->SetLobbyName(NewLobbyName);
		}

		LobbyName = NewLobbyName;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, LobbyName, this);
		BroadcastLobbyInfoUpdatedOnNextTick();
	}
}

void USMLobbyComponent::SetIsPrivateLobby(bool bNewIsPrivateLobby)
{
	if (bIsPrivateLobby != bNewIsPrivateLobby)
	{
		bIsPrivateLobby = bNewIsPrivateLobby;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsPrivateLobby, this);
		BroadcastLobbyInfoUpdatedOnNextTick();
	}
}
