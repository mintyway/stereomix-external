// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMLobbySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnLobbyOwnerChanged, const FUniqueNetIdRepl&, PreviousOwnerId, const FUniqueNetIdRepl&, NewOwnerId);

/**
 * StereoMix Custom Match Lobby Subsystem
 */
UCLASS(DisplayName = "StereoMix Custom Match")
class STEREOMIX_API USMLobbySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USMLobbySubsystem* Get(const UObject* WorldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(BlueprintPure)
	const FUniqueNetIdRepl& GetOwnerId() const { return OwnerId; }

	UFUNCTION(BlueprintPure)
	const FString& GetLobbyName() const { return LobbyName; }

	UFUNCTION(BlueprintPure)
	bool IsPrivateLobby() const { return bIsPrivateLobby; }

	UFUNCTION(BlueprintPure)
	const FString& GetLobbyPassword() const { return LobbyPassword; }
	
	UFUNCTION(BlueprintCallable)
	void SetOwner(const FUniqueNetIdRepl& NewOwnerId);

	UFUNCTION(BlueprintCallable)
	void SetLobbyName(const FString& NewLobbyName);

	UFUNCTION(BlueprintPure)
	bool HasOwner() const;

	UFUNCTION(BlueprintCallable)
	void UpdateLobbySession();

	UPROPERTY(BlueprintAssignable)
	FSMOnLobbyOwnerChanged OnLobbyOwnerChangedEvent;

private:
	FUniqueNetIdRepl OwnerId;

	FString LobbyName;
	bool bIsPrivateLobby;
	FString LobbyPassword;
};
