// Copyright Studio Surround. All Rights Reserved.


#include "SMLobbyPlayerComponent.h"

#include "SMLobbyComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Net/UnrealNetwork.h"
#include "Online/SMPlatformStatics.h"
#include "Player/SMPlayerState.h"

USMLobbyPlayerComponent* USMLobbyPlayerComponent::Get(const APlayerState* InPlayerState)
{
	return InPlayerState ? InPlayerState->GetComponentByClass<USMLobbyPlayerComponent>() : nullptr;
}

USMLobbyPlayerComponent::USMLobbyPlayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void USMLobbyPlayerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsOwner, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsReady, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Platform, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Profile, SharedParams);
}

void USMLobbyPlayerComponent::Reset()
{
	Super::Reset();
	bIsReady = false;
	bIsOwner = false;
	if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
	{
		// StereoMixPS->SetTeam(ESMTeam::None);
		StereoMixPS->SetCharacterType(ESMCharacterType::None);
	}
}

bool USMLobbyPlayerComponent::IsLocalPlayer() const
{
	if (APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (LocalPlayerController->PlayerState == GetPlayerState<ASMPlayerState>())
		{
			return true;
		}
	}
	return false;
}

bool USMLobbyPlayerComponent::CanStartGame() const
{
	if (USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState()))
	{
		return Lobby->CanStartGame();
	}
	return false;
}

void USMLobbyPlayerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		InitLobbyPlayer();
	}

	GetPlayerState<ASMPlayerState>()->GetTeamComponent()->OnTeamChanged.AddUObject(this, &ThisClass::HandlePlayerTeamChanged);
}

void USMLobbyPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority())
	{
		// 플레이어가 로비를 나가는 경우
		if (EndPlayReason == EEndPlayReason::Type::Destroyed)
		{
			if (USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState()))
			{
				// 방장을 다른 플레이어에게 양도합니다.
				if (IsOwner())
				{
					Lobby->ChangeOwner(nullptr);
				}
			}
		}
	}
}

void USMLobbyPlayerComponent::InitLobbyPlayer()
{
	if (HasAuthority())
	{
		if (USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState()))
		{
			// 현재 로비에 오너가 없으면 자신을 방장으로 설정합니다.
			const USMLobbyPlayerComponent* LobbyOwner = Lobby->GetLobbyOwner();
			if (!LobbyOwner)
			{
				Lobby->ChangeOwner(GetPlayerState<ASMPlayerState>());
				LobbyOwner = Lobby->GetLobbyOwner();
			}
			SetOwner(LobbyOwner == this);
		}

		if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
		{
			// StereoMixPS->SetTeam(ESMTeam::None);
			StereoMixPS->SetCharacterType(ESMCharacterType::None);
		}
	}
}

void USMLobbyPlayerComponent::HandlePlayerTeamChanged(AActor* Actor, ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	// BroadcastLobbyPlayerInfoUpdatedOnNextTick();
}

void USMLobbyPlayerComponent::BroadcastLobbyPlayerInfoUpdated()
{
	OnLobbyPlayerInfoUpdatedEvent.Broadcast(this);
}

void USMLobbyPlayerComponent::BroadcastLobbyPlayerInfoUpdatedOnNextTick()
{
	if (!LobbyPlayerInfoUpdateTimerHandle.IsValid())
	{
		LobbyPlayerInfoUpdateTimerHandle = GetWorldTimerManager().SetTimerForNextTick([this] {
			BroadcastLobbyPlayerInfoUpdated();
			LobbyPlayerInfoUpdateTimerHandle.Invalidate();
		});
	}
}

void USMLobbyPlayerComponent::ServerStartGame_Implementation()
{
#if WITH_SERVER_CODE
	if (HasAuthority() && IsOwner())
	{
		USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState());
		Lobby->StartGame();
	}
#endif
}

void USMLobbyPlayerComponent::ServerChangeOwner_Implementation(ASMPlayerState* NewOwnerPlayer)
{
#if WITH_SERVER_CODE
	if (HasAuthority() && IsOwner())
	{
		USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState());
		Lobby->ChangeOwner(NewOwnerPlayer);
	}
#endif
}

void USMLobbyPlayerComponent::SetOwner(bool bIsNewOwner)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		if (bIsOwner != bIsNewOwner)
		{
			bIsOwner = bIsNewOwner;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsOwner, this);
			BroadcastLobbyPlayerInfoUpdatedOnNextTick();

			// 방장이면 준비 상태를 해제합니다.
			if (bIsOwner)
			{
				ServerSetReady(false);
			}
		}
	}
#endif
}

bool USMLobbyPlayerComponent::IsOwner() const
{
	return bIsOwner;
}

void USMLobbyPlayerComponent::ServerSetReady_Implementation(bool bNewReady)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		if (bIsReady != bNewReady)
		{
			bIsReady = bNewReady;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsReady, this);
			BroadcastLobbyPlayerInfoUpdatedOnNextTick();

			// 이 방식이 맘에 들진 않지만 플레이어 레디 변경사항을 알기 위해서는 이 방법이 일단 제일 간단함
			// 대신 위젯에서 일일이 이전사항이랑 비교해서 변경사항이 있을 때만 업데이트하도록 해야함
			if (GetWorld() && GetWorld()->GetGameState())
			{
				if (USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState()))
				{
					Lobby->BroadcastLobbyInfoUpdated();
				}
			}
		}
	}
#endif
}

bool USMLobbyPlayerComponent::IsReady() const
{
	return bIsReady;
}

void USMLobbyPlayerComponent::ServerSetTeam_Implementation(ESMTeam NewTeam)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		if (ASMPlayerState* StereoMixPS = GetPlayerState<ASMPlayerState>())
		{
			if (USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState()))
			{
				if (Lobby->TryChangeTeam(StereoMixPS, NewTeam))
				{
					ServerSetReady(false);
				}
			}
		}
	}
#endif
}

ESMTeam USMLobbyPlayerComponent::GetTeam() const
{
	return GetPlayerState<ASMPlayerState>()->GetTeam();
}

FString USMLobbyPlayerComponent::GetPlayerName() const
{
	return GetPlayerState<ASMPlayerState>()->GetPlayerName();
}

FName USMLobbyPlayerComponent::GetPlatform() const
{
	return Platform;
}

UTexture2D* USMLobbyPlayerComponent::GetPlatformIcon() const
{
	return USMPlatformStatics::GetSpecificPlatformIcon(Platform);
}

FGameplayTag USMLobbyPlayerComponent::GetProfile() const
{
	return Profile;
}

UTexture2D* USMLobbyPlayerComponent::GetProfileImage() const
{
	return nullptr;
}

void USMLobbyPlayerComponent::OnRep_LobbyPlayerInfo()
{
	BroadcastLobbyPlayerInfoUpdatedOnNextTick();
	// 이 방식이 맘에 들진 않지만 플레이어 레디 변경사항을 알기 위해서는 이 방법이 일단 제일 간단함
	// 대신 위젯에서 일일이 이전사항이랑 비교해서 변경사항이 있을 때만 업데이트하도록 해야함
	if (GetWorld() && GetWorld()->GetGameState())
	{
		if (USMLobbyComponent* Lobby = USMLobbyComponent::Get(GetWorld()->GetGameState()))
		{
			Lobby->BroadcastLobbyInfoUpdated();
		}
	}
}
