// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "OnlineSessionSettings.h"
#include "Iris/Core/IrisProfiler.h"

#include "SMGameSession.generated.h"

/**
 * StereoMix Game Session
 */
UCLASS(Config = Game)
class STEREOMIX_API ASMGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	ASMGameSession();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitOptions(const FString& Options) override;
	virtual bool ProcessAutoLogin() override;
	virtual FString ApproveLogin(const FString& Options) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void NotifyLogout(const APlayerController* PC) override;
	virtual void PostSeamlessTravel() override;
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite = false) override;
	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;

protected:
	void CheckAutoDestroySession();

	void StartSession() const;
	void EndSession() const;
	void UpdateSession() const;
	void DestroySession() const;

	virtual void HandleCreateSessionCompleted(FName TargetSessionName, bool bWasSuccessful);
	virtual void HandleRegisterPlayerCompleted(FName TargetSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful);
	virtual void HandleUnregisterPlayerCompleted(FName TargetSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful);
	virtual void HandleStartSessionCompleted(FName TargetSessionName, bool bWasSuccessful);
	virtual void HandleEndSessionCompleted(FName TargetSessionName, bool bWasSuccessful);
	virtual void HandleDestroySessionCompleted(FName TargetSessionName, bool bWasSuccessful);
	virtual void HandleSessionSettingsUpdated(FName Name, const FOnlineSessionSettings& OnlineSessionSettings);

private:
	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle RegisterPlayerDelegateHandle;
	FDelegateHandle UnregisterPlayerDelegateHandle;
	FDelegateHandle StartSessionDelegateHandle;
	FDelegateHandle EndSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
	FDelegateHandle SessionSettingsUpdatedDelegate;

	bool bSessionExists = false;
	int32 NumPlayersInSession = 0;

	bool bIsPrivateSession = false;
	FString SessionPassword;

	bool bIsInGame = false;

	UPROPERTY(Config)
	float AutoDestroyTime = 30.0f;
	FTimerHandle AutoDestroyTimerHandle;
};
