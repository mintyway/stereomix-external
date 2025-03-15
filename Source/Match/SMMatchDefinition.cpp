// Copyright Studio Surround. All Rights Reserved.


#include "SMMatchDefinition.h"

#include "Engine/AssetManager.h"
#include "CommonSessionSubsystem.h"
#include "StereoMixLog.h"
#include "System/SMAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMMatchDefinition)

UCommonSession_HostSessionRequest* CreateHostingRequest(const USMMatchDefinition* Match, const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;

	UCommonSession_HostSessionRequest* Request = GameInstance->GetSubsystem<UCommonSessionSubsystem>()->CreateOnlineHostSessionRequest();
	FString MatchName = Match->GetPrimaryAssetId().PrimaryAssetName.ToString();
	Request->bUseLobbies = false;
	Request->ModeNameForAdvertisement = MatchName;
	Request->MaxPlayerCount = Match->MaxPlayerCount;
	// Request->ExtraArgs.Add(TEXT("Match"), MatchName);
	// Request->ExtraArgs.Add(TEXT("MaxPlayers"), FString::FromInt(Match->MaxPlayerCount));

	return Request;
}

UCommonSession_HostSessionRequest* USMMatchDefinition::CreateHostingRequestForOnlineMatch(const UObject* WorldContextObject) const
{
	UCommonSession_HostSessionRequest* Request = CreateHostingRequest(this, WorldContextObject);

	FAssetData AssetData;
	if (!ensure(UAssetManager::Get().GetPrimaryAssetData(CharacterSelectGameModeId, /*out*/ AssetData)))
	{
		UE_LOG(LogStereoMix, Error, TEXT("Failed to find CharacterSelectGameModeId %s"), *CharacterSelectGameModeId.ToString());
	}
	Request->MapID = CharacterSelectMapId;
	// Request->ExtraArgs.Add(TEXT("ServerType"), TEXT("OnlineMatch"));
	Request->ExtraArgs.Add(TEXT("Game"), USMAssetManager::GetAssetClassPath(CharacterSelectGameModeId));
	return Request;
}

UCommonSession_HostSessionRequest* USMMatchDefinition::CreateHostingRequestForCustomMatch(const UObject* WorldContextObject) const
{
	UCommonSession_HostSessionRequest* Request = CreateHostingRequest(this, WorldContextObject);

	FAssetData AssetData;
	if (!ensure(UAssetManager::Get().GetPrimaryAssetData(CustomMatchLobbyGameModeId, /*out*/ AssetData)))
	{
		UE_LOG(LogStereoMix, Error, TEXT("Failed to find CustomMatchLobbyGameModeId %s"), *CustomMatchLobbyGameModeId.ToString());
	}
	Request->MapID = CustomMatchLobbyMapId;
	// Request->ExtraArgs.Add(TEXT("ServerType"), TEXT("CustomMatch"));
	// 블루프린트 게임모드를 가져오려면 BP_SomeGameMode.BP_SomGameMode_C 형태로 가져와야 함
	Request->ExtraArgs.Add(TEXT("Game"), USMAssetManager::GetAssetClassPath(CustomMatchLobbyGameModeId));
	return Request;
}

FString USMMatchDefinition::GetLobbyUrl() const
{
	FString GameModeName = USMAssetManager::GetAssetClassPath(CustomMatchLobbyGameModeId);
	FString MapName = USMAssetManager::GetAssetName(CustomMatchLobbyMapId).ToString();
	FString URL = FString::Printf(TEXT("%s?Game=%s"), *MapName, *GameModeName);

	// URL += TEXT("?NoSeamlessTravel"); // 무조건 재접속

	// 세션이 닫히는 이슈 때문에 일단 심리스 트래블로 진행
	URL += TEXT("?SeamlessTravel");

	return URL;
}

FString USMMatchDefinition::GetCharacterSelectUrl() const
{
	FString GameModeName = USMAssetManager::GetAssetClassPath(CharacterSelectGameModeId);
	FString MapName = USMAssetManager::GetAssetName(CharacterSelectMapId).ToString();
	FString URL = FString::Printf(TEXT("%s?Game=%s"), *MapName, *GameModeName);

	URL += TEXT("?InGame");
	URL += TEXT("?SeamlessTravel"); // 심리스 트래블

	return URL;
}

FString USMMatchDefinition::GetRoundUrl(int32 Round) const
{
	if (!ensure(Round > 0 || Round <= GetRoundCount()))
	{
		UE_LOG(LogStereoMix, Error, TEXT("Invalid round index %d"), Round);
		return FString();
	}

	FString GameModeName = USMAssetManager::GetAssetClassPath(MatchGameModeId);
	FString MapName = USMAssetManager::GetAssetName(RoundMaps[Round - 1]).ToString();
	FString URL = FString::Printf(TEXT("%s?Game=%s"), *MapName, *GameModeName);

	URL += TEXT("?InGame");
	URL += TEXT("?SeamlessTravel"); // 심리스 트래블
	URL += FString::Printf(TEXT("?Round=%d?PreRoundTime=%f?RoundTime=%f?ResultTime=%f"),
		Round,
		PreRoundTime,
		GameRoundTime,
		ResultTime); // 라운드 정보

	return URL;
}

FPrimaryAssetId USMMatchDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(TEXT("MatchDefinition")), GetFName());
}

int32 USMMatchDefinition::GetRoundCount() const
{
	return RoundMaps.Num();
}
