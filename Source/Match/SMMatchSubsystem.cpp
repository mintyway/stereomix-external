// Copyright Studio Surround. All Rights Reserved.


#include "SMMatchSubsystem.h"

#include "GameFramework/GameModeBase.h"
#include "GameMapsSettings.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "SMMatchDefinition.h"
#include "SMMatchLog.h"
#include "SMMatchStatics.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/SMOnlineDefine.h"
#include "System/SMAssetManager.h"
#include "System/SMGameInstance.h"
#include "System/SMGameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMMatchSubsystem)

namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(MatchPhase, "Phase.Match");
	UE_DEFINE_GAMEPLAY_TAG(MatchPhase_Lobby, "Phase.Match.Lobby");
	UE_DEFINE_GAMEPLAY_TAG(MatchPhase_CharacterSelect, "Phase.Match.CharacterSelect");
	UE_DEFINE_GAMEPLAY_TAG(MatchPhase_Round, "Phase.Match.Round");
}

USMMatchSubsystem::USMMatchSubsystem()
{
	ServerType = ESMMatchServerType::None;
	MatchState = ESMMatchState::Lobby;
}

USMMatchSubsystem* USMMatchSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetGameInstance()->GetSubsystem<USMMatchSubsystem>();
	}
	return nullptr;
}

void USMMatchSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	USMAssetManager& AssetManager = USMAssetManager::Get();
	AssetManager.LoadPrimaryAssetsWithType(TEXT("MatchDefinition"));

	if (GetWorld()->GetGameInstance()->IsDedicatedServerInstance())
	{
		InitMatch();
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
	}
}

void USMMatchSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool USMMatchSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
	// const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	// const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();
	// return bIsServerWorld;
}

ESMMatchState USMMatchSubsystem::UpdateMatchState()
{
	if (GetWorld())
	{
		if (UGameplayStatics::HasOption(GetWorld()->GetAuthGameMode()->OptionsString, TEXT("InGame")))
		{
			MatchState = ESMMatchState::InProgress;
		}
		else
		{
			MatchState = ESMMatchState::Lobby;
		}
	}

	return MatchState;
}

void USMMatchSubsystem::InitMatch()
{
	FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance || World->GetNetMode() != NM_DedicatedServer || World->URL.Map != DefaultMap)
	{
		return;
	}

	const TCHAR* CommandLine = FCommandLine::Get();

	FString MatchTypeFromCommandLine;
	if (!FParse::Value(CommandLine, TEXT("Match="), MatchTypeFromCommandLine))
	{
		UE_LOG(LogStereoMixMatch, Warning, TEXT("Match type not specified on command line, using default."));
		MatchTypeFromCommandLine = TEXT("DefaultMatch");
	}

	USMMatchDefinition* FoundMatchDefinition = USMMatchStatics::FindMatchDefinition(FName(MatchTypeFromCommandLine));
	if (!FoundMatchDefinition)
	{
		UE_LOG(LogStereoMixMatch, Error, TEXT("Match definition not found for %s."), *MatchTypeFromCommandLine);
		return;
	}

	MatchDefinition = FoundMatchDefinition;

	if (FParse::Param(CommandLine, TEXT("CustomMatch")))
	{
		ServerType = ESMMatchServerType::CustomMatch;
	}
	else if (FParse::Param(CommandLine, TEXT("OnlineMatch")))
	{
		ServerType = ESMMatchServerType::OnlineMatch;
	}
	else
	{
		UE_LOG(LogStereoMixMatch, Warning, TEXT("Match type not specified on command line, set default to CustomMatch."));
		ServerType = ESMMatchServerType::CustomMatch;
	}
}

void USMMatchSubsystem::UpdateMatchSession() const
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

	FOnlineSessionSettings* SessionsSettings = Sessions->GetSessionSettings(NAME_GameSession);
	if (!SessionsSettings)
	{
		return;
	}

	SessionsSettings->Set(STEREOMIX_MATCH_VERSION, USMGameplayStatics::GetProjectVersion(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionsSettings->Set(STEREOMIX_MATCH_MAP, GetWorld()->GetMapName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionsSettings->Set(STEREOMIX_MATCH_MODE, MatchId.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Sessions->UpdateSession(NAME_GameSession, *SessionsSettings, true);
}

void USMMatchSubsystem::HandlePostLoadMap(UWorld* World) const
{
	UpdateMatchSession();
}
