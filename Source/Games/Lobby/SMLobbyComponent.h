// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "Player/SMPlayerState.h"
#include "UI/Common/SMPrimaryLayout.h"
#include "SMLobbyComponent.generated.h"

class USMLobbyPlayerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSMOnLobbyInfoUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSMOnLobbyPlayersUpdated);

UCLASS(Abstract, DisplayName = "StereoMix Lobby Component")
class STEREOMIX_API USMLobbyComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	static USMLobbyComponent* Get(const AGameStateBase* InGameState);
	
	USMLobbyComponent(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 게임 시작 관련 기능은 LobbyPlayer에 있는 함수 사용하기
	
	void StartGame();
	bool CanStartGame() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	TArray<USMLobbyPlayerComponent*> GetTeamPlayers(ESMTeam Team) const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	USMLobbyPlayerComponent* GetLocalLobbyPlayer() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	FString GetLobbyName() const { return LobbyName; }

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	bool IsPrivateLobby() const { return bIsPrivateLobby; }

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	int32 GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	int32 GetCurrentPlayers() const { return CurrentPlayers; }

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	USMLobbyPlayerComponent* GetLobbyOwner() const;

	void ChangeOwner(APlayerState* NewOwner);

	bool TryChangeTeam(ASMPlayerState* Player, ESMTeam NewTeam);
	
	UFUNCTION(Exec)
	void SetLobbyName(const FString& NewLobbyName);
	
	void SetIsPrivateLobby(bool bNewIsPrivateLobby);

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Lobby", DisplayName = "On Lobby Info Updated")
	FSMOnLobbyInfoUpdated OnLobbyInfoUpdatedEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Lobby", DisplayName = "On Lobby Players Updated")
	FSMOnLobbyPlayersUpdated OnLobbyPlayersUpdatedEvent;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void StartLobby();

	void SetMaxPlayers(int32 NewMaxPlayers);
	void SetCurrentPlayers(int32 NewCurrentPlayers);

	void AddPlayerToTeam(const ASMPlayerState* Player, ESMTeam Team);
	void RemovePlayerFromTeam(const ASMPlayerState* Player, ESMTeam Team);

	void HandlePlayerAdded(ASMPlayerState* NewPlayer);
	void HandlePlayerRemoved(ASMPlayerState* RemovedPlayer);
	void HandlePlayerTeamChanged(AActor* Actor, ESMTeam PreviousTeam, ESMTeam NewTeam);

	void BroadcastLobbyInfoUpdated();
	void BroadcastLobbyInfoUpdatedOnNextTick();
	void BroadcastLobbyPlayersUpdated();
	void BroadcastLobbyPlayersUpdatedOnNextTick();

private:
	UFUNCTION()
	void OnRep_LobbyInfo();

	UFUNCTION()
	void OnRep_LobbyPlayers();

	// UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Lobby")
	// TSoftClassPtr<USMPrimaryLayout> LobbyLayoutClass;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyInfo)
	FString LobbyName;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyInfo)
	bool bIsPrivateLobby = false;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyInfo)
	TWeakObjectPtr<ASMPlayerState> LobbyOwner;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyInfo)
	int32 MaxPlayers = 0;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyInfo)
	int32 CurrentPlayers = 0;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayers)
	TArray<TObjectPtr<USMLobbyPlayerComponent>> TeamPlayers_EDM;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayers)
	TArray<TObjectPtr<USMLobbyPlayerComponent>> TeamPlayers_FB;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayers)
	TArray<TObjectPtr<USMLobbyPlayerComponent>> NonTeamPlayers;

	bool bPendingNextMap = false;
	FTimerHandle LobbyInfoBroadcastTimerHandle;
	FTimerHandle LobbyPlayersBroadcastTimerHandle;

	friend class USMLobbyPlayerComponent;
};
