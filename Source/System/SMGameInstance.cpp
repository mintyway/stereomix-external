﻿// Copyright Studio Surround. All Rights Reserved.

#include "SMGameInstance.h"

#include "CommonUserSubsystem.h"
#include "HttpModule.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "SMGameplayStatics.h"
#include "StereoMixLog.h"
#include "StereoMixSDKModule.h"
#include "Data/DataTable/SMCharacterData.h"
#include "Interfaces/IHttpResponse.h"
#include "Internationalization/StringTableRegistry.h"
#include "Player/SMLocalPlayer.h"
#include "Settings/SMSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameInstance)

#define LOCTABLE_ID "Game"

void USMGameInstance::Init()
{
	UE_LOG(LogStereoMix, Warning, TEXT("Build Version: %s"), *USMGameplayStatics::GetBuildVersion());
	UE_LOG(LogStereoMix, Warning, TEXT("Project Version: %s"), *USMGameplayStatics::GetProjectVersion());

	SyncCharacterStatsWithServer();

	Super::Init();
}

void USMGameInstance::StartGameInstance()
{
	// 플랫폼 온라인서브시스템 정상 초기화 여부 체크
	FString NativePlatform;
	if (GConfig->GetString(TEXT("OnlineSubsystem"), TEXT("NativePlatformService"), NativePlatform, GEngineIni))
	{
		if (IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld()))
		{
			FName OnlineSubsystemName = OnlineSub->GetSubsystemName();
			if (NativePlatform == "Steam" && OnlineSubsystemName == "Null")
			{
				FMessageDialog::Open(EAppMsgCategory::Error, EAppMsgType::Ok, LOCTABLE(LOCTABLE_ID, "NoSteam"));
				FGenericPlatformMisc::RequestExit(false);
			}
		}
	}

	Super::StartGameInstance();
}

void USMGameInstance::OnStart()
{
	Super::OnStart();
#if !UE_SERVER
	USMSettingsLocal::Get()->ApplyNonResolutionSettings();
#endif
}

void USMGameInstance::ReturnToMainMenu()
{
	Super::ReturnToMainMenu();

	// TODO: 세션, 로비 연결 끊기
}

void USMGameInstance::AddGlobalTag(const FGameplayTag Tag)
{
	GlobalTags.AddTag(Tag);
}

void USMGameInstance::SyncCharacterStatsWithServer()
{
#if WITH_EDITOR
	if (!bUseServerDataTable)
	{
		return;
	}
#endif

	if (const UWorld* World = GetWorld(); !World || World->GetNetMode() != NM_DedicatedServer)
	{
		return;
	}

	UE_LOG(LogStereoMix, Log, TEXT("Requesting DataTable from Server"));

	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();

	const FStereoMixSDKModule& SDK = FStereoMixSDKModule::Get();
	const FString Endpoint = SDK.GetServerEndpoint();

	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::ReceivedDataTableFromServer);
	Request->SetURL(FString::Printf(TEXT("%s/stats/character"), *Endpoint));
	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}

FSMCharacterStatsData* USMGameInstance::GetCharacterStatsData(ESMCharacterType CharacterType)
{
	return CharacterStatsDataTable->FindRow<FSMCharacterStatsData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterAttackData* USMGameInstance::GetCharacterAttackData(ESMCharacterType CharacterType)
{
	return CharacterAttackDataTable->FindRow<FSMCharacterAttackData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterSkillData* USMGameInstance::GetCharacterSkillData(ESMCharacterType CharacterType)
{
	return CharacterSkillDataTable->FindRow<FSMCharacterSkillData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterNoiseBreakData* USMGameInstance::GetCharacterNoiseBreakData(ESMCharacterType CharacterType)
{
	return CharacterNoiseBreakDataTable->FindRow<FSMCharacterNoiseBreakData>(CharacterTypeToName(CharacterType), TEXT(""));
}

void USMGameInstance::HandleUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	Super::HandleUserInitialized(UserInfo, bSuccess, Error, RequestedPrivilege, OnlineContext);
	// If login succeeded, tell the local player to load their settings
	if (bSuccess && ensure(UserInfo))
	{
		// There will not be a local player attached to the dedicated server user
		if (USMLocalPlayer* LocalPlayer = Cast<USMLocalPlayer>(GetLocalPlayerByIndex(UserInfo->LocalPlayerIndex)))
		{
			LocalPlayer->LoadSharedSettingsFromDisk();
		}
	}
}

template<typename DataType, typename FieldType>
void USMGameInstance::UpdateDataTable(TSharedPtr<FJsonObject> CharacterData, UDataTable* DataTable, const FString& CharacterName, const TMap<FString, FieldType DataType::*>& FieldMap)
{
	DataType* Data = DataTable->FindRow<DataType>(FName(CharacterName), TEXT(""));
	if (CharacterData.IsValid() && Data)
	{
		for (const auto& Field : FieldMap)
		{
			if (CharacterData->HasField(Field.Key))
			{
				if constexpr (std::is_same_v<FieldType, float>)
				{
					Data->*(Field.Value) = CharacterData->GetNumberField(Field.Key);
				}
				else if constexpr (std::is_same_v<FieldType, int32>)
				{
					Data->*(Field.Value) = CharacterData->GetIntegerField(Field.Key);
				}
			}
		}

#if WITH_EDITOR
		// 변경 내용을 저장
		if (UPackage* Package = DataTable->GetOutermost())
		{
			Package->SetDirtyFlag(true);
		}
#endif
	}
}

void USMGameInstance::ReceivedDataTableFromServer(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	const bool bResponseStatusOK = bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200;
	if (!bResponseStatusOK)
	{
		UE_LOG(LogStereoMix, Error, TEXT("Failed to fetch DataTable from Server"));
		// FMessageDialog::Open(EAppMsgCategory::Error, EAppMsgType::Ok, LOCTABLE(LOCTABLE_ID, "StatTable_FetchFailed"));
		// FGenericPlatformMisc::RequestExit(false);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogStereoMix, Log, TEXT("DataTable received from Server"));

		for (auto& CharacterItem : JsonObject->Values)
		{
			FString CharacterName = CharacterItem.Key;
			TSharedPtr<FJsonObject> CharacterData = CharacterItem.Value->AsObject();

			if (CharacterData.IsValid())
			{
				// Stats Data
				TMap<FString, float FSMCharacterStatsData::*> StatsFieldMap = {
					{ TEXT("HP"), &FSMCharacterStatsData::HP },
					{ TEXT("MoveSpeed"), &FSMCharacterStatsData::MoveSpeed },
					{ TEXT("Stamina"), &FSMCharacterStatsData::Stamina },
					{ TEXT("SkillGauge"), &FSMCharacterStatsData::SkillGauge }
				};
				UpdateDataTable<FSMCharacterStatsData, float>(CharacterData->GetObjectField(TEXT("Stats")), CharacterStatsDataTable, CharacterName, StatsFieldMap);

				// Attack Data
				TMap<FString, float FSMCharacterAttackData::*> AttackFieldMap = {
					{ TEXT("Damage"), &FSMCharacterAttackData::Damage },
					{ TEXT("DistanceByTile"), &FSMCharacterAttackData::DistanceByTile },
					{ TEXT("ProjectileSpeed"), &FSMCharacterAttackData::ProjectileSpeed },
					{ TEXT("AttackPerSecond"), &FSMCharacterAttackData::AttackPerSecond },
					{ TEXT("SpreadAngle"), &FSMCharacterAttackData::SpreadAngle }
				};
				UpdateDataTable<FSMCharacterAttackData, float>(CharacterData->GetObjectField(TEXT("Attack")), CharacterAttackDataTable, CharacterName, AttackFieldMap);

				// Skill Data
				TMap<FString, float FSMCharacterSkillData::*> SkillFieldMap = {
					{ TEXT("Damage"), &FSMCharacterSkillData::Damage },
					{ TEXT("DistanceByTile"), &FSMCharacterSkillData::DistanceByTile },
					{ TEXT("ProjectileSpeed"), &FSMCharacterSkillData::ProjectileSpeed },
					{ TEXT("StartUpTime"), &FSMCharacterSkillData::StartUpTime },
					{ TEXT("Range"), &FSMCharacterSkillData::Range },
					{ TEXT("Magnitude"), &FSMCharacterSkillData::Magnitude },
					{ TEXT("Duration"), &FSMCharacterSkillData::Duration }
				};
				UpdateDataTable<FSMCharacterSkillData, float>(CharacterData->GetObjectField(TEXT("Skill")), CharacterSkillDataTable, CharacterName, SkillFieldMap);

				// Noise Break Data (float fields)
				TMap<FString, float FSMCharacterNoiseBreakData::*> NoiseBreakFieldMap = {
					{ TEXT("Damage"), &FSMCharacterNoiseBreakData::Damage },
					{ TEXT("DistanceByTile"), &FSMCharacterNoiseBreakData::DistanceByTile },
					{ TEXT("GravityScale"), &FSMCharacterNoiseBreakData::GravityScale },
					{ TEXT("ApexHeight"), &FSMCharacterNoiseBreakData::ApexHeight }
				};
				UpdateDataTable<FSMCharacterNoiseBreakData, float>(CharacterData->GetObjectField(TEXT("NoiseBreak")), CharacterNoiseBreakDataTable, CharacterName, NoiseBreakFieldMap);

				// Noise Break Data (int32 fields)
				TMap<FString, int32 FSMCharacterNoiseBreakData::*> NoiseBreakIntFieldMap = {
					{ TEXT("CaptureSize"), &FSMCharacterNoiseBreakData::CaptureSize }
				};
				UpdateDataTable<FSMCharacterNoiseBreakData, int32>(CharacterData->GetObjectField(TEXT("NoiseBreak")), CharacterNoiseBreakDataTable, CharacterName, NoiseBreakIntFieldMap);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JSON 파싱 오류"));
	}
}

FName USMGameInstance::CharacterTypeToName(ESMCharacterType CharacterType)
{
	switch (CharacterType)
	{
		case ESMCharacterType::ElectricGuitar:
			return TEXT("ElectricGuitar");
		case ESMCharacterType::Piano:
			return TEXT("Piano");
		case ESMCharacterType::Bass:
			return TEXT("Bass");
		default:
			return NAME_None;
	}
}

#undef LOCTABLE_ID