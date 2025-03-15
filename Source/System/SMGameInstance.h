// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "GameplayTagContainer.h"
#include "Data/SMCharacterType.h"
#include "Interfaces/IHttpRequest.h"

#include "SMGameInstance.generated.h"

enum class ECommonUserOnlineContext : uint8;
struct FGameplayTagContainer;
struct FSMCharacterNoiseBreakData;
struct FSMCharacterSkillData;
struct FSMCharacterAttackData;
struct FSMCharacterStatsData;
class FHttpModule;


UCLASS(Abstract)
class STEREOMIX_API USMGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void StartGameInstance() override;

	virtual void OnStart() override;

	virtual void ReturnToMainMenu() override;

	FGameplayTagContainer& GetGlobalTags() { return GlobalTags; }

	void AddGlobalTag(FGameplayTag Tag);

	void SyncCharacterStatsWithServer();

	FSMCharacterStatsData* GetCharacterStatsData(ESMCharacterType CharacterType);

	FSMCharacterAttackData* GetCharacterAttackData(ESMCharacterType CharacterType);

	FSMCharacterSkillData* GetCharacterSkillData(ESMCharacterType CharacterType);

	FSMCharacterNoiseBreakData* GetCharacterNoiseBreakData(ESMCharacterType CharacterType);

	const TArray<FSoftObjectPath>& GetStageLevels() const { return StageLevels; }

	virtual void HandleUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext) override;

protected:
	template<typename DataType, typename FieldType>
	void UpdateDataTable(TSharedPtr<FJsonObject> CharacterData, UDataTable* DataTable, const FString& CharacterName, const TMap<FString, FieldType DataType::*>& FieldMap);

	void ReceivedDataTableFromServer(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	static FName CharacterTypeToName(ESMCharacterType CharacterType);

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseServerDataTable:1;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterStatsDataTable;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterAttackDataTable;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterSkillDataTable;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterNoiseBreakDataTable;

	UPROPERTY(EditAnywhere, Category = "Design|Level", meta = (MetaClass = "/Script/Engine.World"))
	TArray<FSoftObjectPath> StageLevels;

private:
	FGameplayTagContainer GlobalTags;
};
