// Copyright Studio Surround. All Rights Reserved.


#include "SMLobbySubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "SMLobbyLog.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/SMOnlineDefine.h"

USMLobbySubsystem* USMLobbySubsystem::Get(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetGameInstance()->GetSubsystem<USMLobbySubsystem>();
	}
	return nullptr;
}

void USMLobbySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!FParse::Value(FCommandLine::Get(), TEXT("LobbyName="), LobbyName))
	{
		LobbyName = TEXT("DefaultLobby");
	}

	if (FParse::Bool(FCommandLine::Get(), TEXT("-Private"), bIsPrivateLobby))
	{
		UE_LOG(LogStereoMixLobby, Log, TEXT("Private lobby enabled."));
		if (!FParse::Value(FCommandLine::Get(), TEXT("Password="), LobbyPassword))
		{
			// TODO: 패스워드가 빠졌을 때 처리방법을 생각해봐야 함
			UE_LOG(LogStereoMixLobby, Error, TEXT("Private session requires password. set default password to '1234'."));
			LobbyPassword = TEXT("1234");
		}
	}
}

void USMLobbySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool USMLobbySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	return GameInstance->IsDedicatedServerInstance();
// #if WITH_EDITOR
// 	return true;
// #else
// #endif
}

void USMLobbySubsystem::SetOwner(const FUniqueNetIdRepl& NewOwnerId)
{
	if (OwnerId != NewOwnerId)
	{
		OwnerId = NewOwnerId;
	}
}

void USMLobbySubsystem::SetLobbyName(const FString& NewLobbyName)
{
	LobbyName = NewLobbyName;
	UpdateLobbySession();
}

bool USMLobbySubsystem::HasOwner() const
{
	return OwnerId.IsValid();
}

void USMLobbySubsystem::UpdateLobbySession()
{
	IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld());
	if (!Sessions)
	{
		return;
	}

	if (Sessions->GetSessionState(NAME_GameSession) == EOnlineSessionState::NoSession)
	{
		return;
	}

	FOnlineSessionSettings* SessionSettings = Sessions->GetSessionSettings(NAME_GameSession);
	if (!SessionSettings)
	{
		return;
	}

	SessionSettings->Set(STEREOMIX_LOBBY_NAME, LobbyName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Sessions->UpdateSession(NAME_GameSession, *SessionSettings, true);
}
