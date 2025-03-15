// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "Player/SMPlayerState.h"
#include "SMLobbyPlayerComponent.generated.h"

class ASMPlayerState;
class USMLobbyPlayerComponent;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnPlayerOwnerChanged, ASMPlayerState*, Player, bool, bIsOwner);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnPlayerReadyChanged, ASMPlayerState*, Player, bool, bIsReady);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSMOnPlayerTeamChanged, ASMPlayerState*, Player, ESMTeam, PreviousTeam,  ESMTeam, NewTeam);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnLobbyPlayerInfoUpdated, USMLobbyPlayerComponent*, LobbyPlayer);

UCLASS(DisplayName = "StereoMix Lobby Player Component", meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMLobbyPlayerComponent : public UPlayerStateComponent
{
	GENERATED_BODY()

public:
	static USMLobbyPlayerComponent* Get(const APlayerState* InPlayerState);

	USMLobbyPlayerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Reset() override;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	bool IsLocalPlayer() const;
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "StereoMix|Lobby", DisplayName = "Start Game")
	void ServerStartGame();

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	bool CanStartGame() const;
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "StereoMix|Lobby", DisplayName = "Change Owner")
	void ServerChangeOwner(ASMPlayerState* NewOwnerPlayer);

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	bool IsOwner() const;

	void SetOwner(bool bIsNewOwner);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "StereoMix|Lobby", DisplayName = "Set Ready")
	void ServerSetReady(bool bNewReady);

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	bool IsReady() const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "StereoMix|Lobby", DisplayName = "Set Team")
	void ServerSetTeam(ESMTeam NewTeam);
	
	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	ESMTeam GetTeam() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	FString GetPlayerName() const;
	
	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	FName GetPlatform() const;
	
	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	UTexture2D* GetPlatformIcon() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	FGameplayTag GetProfile() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Lobby")
	UTexture2D* GetProfileImage() const;
	
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Lobby", DisplayName = "On Lobby Player Info Updated")
	FSMOnLobbyPlayerInfoUpdated OnLobbyPlayerInfoUpdatedEvent;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitLobbyPlayer();

	void HandlePlayerTeamChanged(AActor* Actor, ESMTeam PreviousTeam, ESMTeam NewTeam);
	void BroadcastLobbyPlayerInfoUpdated();
	void BroadcastLobbyPlayerInfoUpdatedOnNextTick();

private:
	UFUNCTION()
	void OnRep_LobbyPlayerInfo();
	
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayerInfo)
	bool bIsOwner = false;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayerInfo)
	bool bIsReady = false;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayerInfo)
	FName Platform;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_LobbyPlayerInfo)
	FGameplayTag Profile;

	FTimerHandle LobbyPlayerInfoUpdateTimerHandle;
};
