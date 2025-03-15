// Copyright Studio Surround. All Rights Reserved.


#include "SMGameSession.h"

#include "GameFramework/GameModeBase.h"
#include "GameMapsSettings.h"
#include "OnlineSubsystemUtils.h"
#include "SMGameInstance.h"
#include "SMGameplayStatics.h"
#include "Games/Lobby/SMLobbySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Match/SMDeploymentSubsystem.h"
#include "Match/SMMatchDefinition.h"
#include "Match/SMMatchSubsystem.h"
#include "Online/SMOnlineDefine.h"
#include "Online/SMSessionSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixSession, Log, All);


ASMGameSession::ASMGameSession()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASMGameSession::BeginPlay()
{
	// BeginPlay는 심리스 트래블 포함 무조건 월드 당 한번 호출됩니다.

	Super::BeginPlay();
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASMGameSession::BeginPlay, NumPlayers: %d"), GetWorld()->GetAuthGameMode()->GetNumPlayers());
		if (GetWorld()->GetAuthGameMode()->GetNumPlayers() == 0)
		{
			// Deployment일 때만 AutoDestroyTime이 적용됩니다.
			if (USMDeploymentSubsystem* Deployment = USMDeploymentSubsystem::Get(this))
			{
				if (Deployment->IsDeployment())
				{
					// 플레이어가 없는 경우 일정 시간 후에 게임을 종료합니다.
					GetWorldTimerManager().SetTimer(AutoDestroyTimerHandle, [this] {
						UE_LOG(LogTemp, Warning, TEXT("Auto destroy session."));
						DestroySession();
					}, AutoDestroyTime, false);
				}
			}
		}

		if (IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld()))
		{
			StartSessionDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(this, &ASMGameSession::HandleStartSessionCompleted));
			EndSessionDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(FOnEndSessionCompleteDelegate::CreateUObject(this, &ASMGameSession::HandleEndSessionCompleted));
			DestroySessionDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ASMGameSession::HandleDestroySessionCompleted));
			RegisterPlayerDelegateHandle = Sessions->AddOnRegisterPlayersCompleteDelegate_Handle(FOnRegisterPlayersCompleteDelegate::CreateUObject(this, &ASMGameSession::HandleRegisterPlayerCompleted));
			UnregisterPlayerDelegateHandle = Sessions->AddOnUnregisterPlayersCompleteDelegate_Handle(FOnUnregisterPlayersCompleteDelegate::CreateUObject(this, &ASMGameSession::HandleUnregisterPlayerCompleted));
			SessionSettingsUpdatedDelegate = Sessions->AddOnSessionSettingsUpdatedDelegate_Handle(FOnSessionSettingsUpdatedDelegate::CreateUObject(this, &ASMGameSession::HandleSessionSettingsUpdated));
		}

		FString Options = GetWorld()->GetAuthGameMode()->OptionsString;
		if (UGameplayStatics::HasOption(Options, TEXT("InGame")))
		{
			bIsInGame = true;
			// StartSession();
		}
		else
		{
			// EndSession();
		}

		UpdateSession();
	}
}

void ASMGameSession::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		UE_LOG(LogStereoMixSession, Warning, TEXT("ASMGameSession::EndPlay"));
		if (AutoDestroyTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(AutoDestroyTimerHandle);
		}

		if (IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld()))
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionDelegateHandle);
			StartSessionDelegateHandle.Reset();
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(EndSessionDelegateHandle);
			EndSessionDelegateHandle.Reset();
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
			DestroySessionDelegateHandle.Reset();
			Sessions->ClearOnRegisterPlayersCompleteDelegate_Handle(RegisterPlayerDelegateHandle);
			RegisterPlayerDelegateHandle.Reset();
			Sessions->ClearOnUnregisterPlayersCompleteDelegate_Handle(UnregisterPlayerDelegateHandle);
			UnregisterPlayerDelegateHandle.Reset();
			Sessions->ClearOnSessionSettingsUpdatedDelegate_Handle(SessionSettingsUpdatedDelegate);
			SessionSettingsUpdatedDelegate.Reset();
		}
	}
}

void ASMGameSession::InitOptions(const FString& Options)
{
	// InitOptions는 심리스 트래블 시 호출되지 않습니다.

	UWorld* const World = GetWorld();
	check(World);

	SessionName = NAME_GameSession;

	// AGameSession은 Standalone일 때 클라이언트에서도 호출되므로(Authority 권한을 가지고 있기 때문에) 이 점 주의해야 함

	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		// PrivateSession 설정
		if (const USMLobbySubsystem* Lobby = USMLobbySubsystem::Get(this))
		{
			bIsPrivateSession = Lobby->IsPrivateLobby();
			SessionPassword = Lobby->GetLobbyPassword();
		}

		// MaxPlayers 설정
		if (USMMatchSubsystem* MatchSubsystem = USMMatchSubsystem::Get(this))
		{
			if (const USMMatchDefinition* MatchDefinition = MatchSubsystem->GetMatchDefinition())
			{
				MaxPlayers = MatchDefinition->MaxPlayerCount;
			}
		}
	}
}

bool ASMGameSession::ProcessAutoLogin()
{
	// EOS에서 TrustedServer Policy를 가진 데디케이티드 서버는 로그인할 필요가 없습니다.
	return false;
}

FString ASMGameSession::ApproveLogin(const FString& Options)
{
	// 패스워드 체크
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		if (bIsPrivateSession)
		{
			if (UGameplayStatics::HasOption(Options, TEXT("Password")))
			{
				FString InputPassword = UGameplayStatics::ParseOption(Options, TEXT("Password"));
				if (InputPassword != SessionPassword)
				{
					return TEXT("Invalid password.");
				}
			}
			else
			{
				return TEXT("Invalid password.");
			}
		}

		if (bIsInGame)
		{
			return TEXT("Match is in progress.");
		}

		if (NumPlayersInSession == MaxPlayers)
		{
			return TEXT("Server full.");
		}
	}
	return Super::ApproveLogin(Options);
}

void ASMGameSession::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		NumPlayersInSession++;
		UE_LOG(LogStereoMixSession, Warning, TEXT("PostLogin: NumPlayersInSession++; NumPlayersInSession: %d"), NumPlayersInSession);

		if (AutoDestroyTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(AutoDestroyTimerHandle);
		}
	}
}

void ASMGameSession::NotifyLogout(const APlayerController* PC)
{
	Super::NotifyLogout(PC);
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		NumPlayersInSession--;
		UE_LOG(LogStereoMixSession, Warning, TEXT("NotifyLogout: NumPlayersInSession--; NumPlayersInSession: %d"), NumPlayersInSession);
		if (NumPlayersInSession <= 0 && !GetWorld()->IsInSeamlessTravel())
		{
			// 만약 매치메이킹으로 진행되는 게임이라면 플레이어들이 들어오는 중에 전부 나간 후에 바로 들어오는 플레이어가 있는 경우가 있을 수도? 
			DestroySession();
			UE_LOG(LogTemp, Warning, TEXT("Destroy Session because there is no player in session"));
		}
	}
}

void ASMGameSession::PostSeamlessTravel()
{
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		NumPlayersInSession = GetWorld()->GetNumPlayerControllers();
		UE_LOG(LogTemp, Warning, TEXT("NumPlayersInSession: %d, GameMode: %d"), NumPlayersInSession, GetWorld()->GetAuthGameMode()->GetNumPlayers());
	}
}

void ASMGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	// 부모 함수에서 Session RegisterPlayer로 호출함
}

void ASMGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);
}

void ASMGameSession::CheckAutoDestroySession()
{
	if (NumPlayersInSession <= 0)
	{
		DestroySession();
	}
}

void ASMGameSession::StartSession() const
{
	IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld());
	if (!Sessions)
	{
		return;
	}

	if (Sessions->GetSessionState(SessionName) == EOnlineSessionState::NoSession)
	{
		return;
	}

	if (Sessions->GetSessionState(SessionName) == EOnlineSessionState::InProgress)
	{
		UE_LOG(LogStereoMixSession, Log, TEXT("Session is already in progress."));
	}

	Sessions->StartSession(SessionName);
}

void ASMGameSession::EndSession() const
{
	IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld());
	if (!Sessions)
	{
		return;
	}

	if (Sessions->GetSessionState(SessionName) == EOnlineSessionState::NoSession)
	{
		return;
	}

	if (Sessions->GetSessionState(SessionName) != EOnlineSessionState::InProgress)
	{
		UE_LOG(LogStereoMixSession, Log, TEXT("Session is not in progress."));
	}

	Sessions->EndSession(SessionName);
}

void ASMGameSession::UpdateSession() const
{
	IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld());
	if (!Sessions)
	{
		return;
	}

	if (Sessions->GetSessionState(SessionName) == EOnlineSessionState::NoSession)
	{
		return;
	}

	FOnlineSessionSettings* SessionSettings = Sessions->GetSessionSettings(SessionName);
	if (!SessionSettings)
	{
		return;
	}

	if (USMMatchSubsystem* Match = USMMatchSubsystem::Get(this))
	{
		SessionSettings->Set(STEREOMIX_MATCH_VERSION, USMGameplayStatics::GetProjectVersion(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings->Set(STEREOMIX_MATCH_STATE, static_cast<int32>(Match->UpdateMatchState()), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings->Set(STEREOMIX_MATCH_MAP, GetWorld()->GetMapName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings->Set(STEREOMIX_MATCH_MODE, Match->GetMatchId().ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		Sessions->UpdateSession(SessionName, *SessionSettings, true);
	}
}

void ASMGameSession::DestroySession() const
{
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		if (IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld()))
		{
			Sessions->DestroySession(SessionName);
		}
	}
}

void ASMGameSession::HandleCreateSessionCompleted(FName TargetSessionName, bool bWasSuccessful)
{
	if (TargetSessionName != SessionName)
	{
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// Nothing special here, simply log that the session is created.
	if (bWasSuccessful)
	{
		bSessionExists = true;
		UE_LOG(LogStereoMixSession, Warning, TEXT("Session created!"));
	}
	else
	{
		UE_LOG(LogStereoMixSession, Error, TEXT("Failed to create session!"));
	}

	// Clear our handle and reset the delegate. 
	Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	CreateSessionDelegateHandle.Reset();
}

void ASMGameSession::HandleRegisterPlayerCompleted(FName TargetSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful)
{
	if (TargetSessionName != SessionName)
	{
		return;
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogStereoMixSession, Warning, TEXT("Player registered."));
		for (const FUniqueNetIdRef& PlayerId : PlayerIds)
		{
			UE_LOG(LogStereoMixSession, Log, TEXT("Registered PlayerId: %s"), *PlayerId->ToString());
		}
	}
	else
	{
		UE_LOG(LogStereoMixSession, Error, TEXT("Failed to register player."));
	}
}

void ASMGameSession::HandleUnregisterPlayerCompleted(FName TargetSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful)
{
	if (TargetSessionName != SessionName)
	{
		return;
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogStereoMixSession, Warning, TEXT("Player unregistered."));
		for (const FUniqueNetIdRef& PlayerId : PlayerIds)
		{
			UE_LOG(LogStereoMixSession, Log, TEXT("Unregistered PlayerId: %s"), *PlayerId->ToString());
		}
	}
	else
	{
		UE_LOG(LogStereoMixSession, Error, TEXT("Failed to unregister player."));
	}
}

void ASMGameSession::HandleStartSessionCompleted(FName TargetSessionName, bool bWasSuccessful)
{
	if (TargetSessionName != SessionName)
	{
		return;
	}
	if (bWasSuccessful)
	{
		UE_LOG(LogStereoMixSession, Warning, TEXT("Session started."));
	}
	else
	{
		UE_LOG(LogStereoMixSession, Error, TEXT("Failed to start session."));
	}
}

void ASMGameSession::HandleEndSessionCompleted(FName TargetSessionName, bool bWasSuccessful)
{
	if (TargetSessionName != SessionName)
	{
		return;
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogStereoMixSession, Warning, TEXT("Session ended."));
	}
	else
	{
		UE_LOG(LogStereoMixSession, Error, TEXT("Failed to end session."));
	}
}

void ASMGameSession::HandleDestroySessionCompleted(FName TargetSessionName, bool bWasSuccessful)
{
	if (TargetSessionName != SessionName)
	{
		return;
	}

	if (bWasSuccessful)
	{
		UE_LOG(LogStereoMixSession, Warning, TEXT("Session destroyed."));

		// Deployment인 경우 Deployment 제거 요청
		if (USMDeploymentSubsystem* Deployment = USMDeploymentSubsystem::Get(this))
		{
			if (Deployment->IsDeployment())
			{
				Deployment->Delete();
				return;
			}
		}

		if (USMMatchSubsystem* MatchSubsystem = USMMatchSubsystem::Get(this))
		{
			if (MatchSubsystem->GetServerType() == ESMMatchServerType::CustomMatch)
			{
				FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
				GetWorld()->ServerTravel(DefaultMap, true);
			}
		}
	}
	else
	{
		UE_LOG(LogStereoMixSession, Error, TEXT("Failed to destroy session."));
	}
}

void ASMGameSession::HandleSessionSettingsUpdated(FName Name, const FOnlineSessionSettings& OnlineSessionSettings)
{
	if (Name != SessionName)
	{
		return;
	}

	UE_LOG(LogStereoMixSession, Warning, TEXT("Session settings updated."));
	DumpSessionSettings(&OnlineSessionSettings);
}
