// Copyright Studio Surround. All Rights Reserved.


#include "SMMatchStatics.h"

#include "SMMatchDefinition.h"
#include "System/SMAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMMatchStatics)

TArray<UObject*> USMMatchStatics::GetMatchDefinitions()
{
	static const FPrimaryAssetType MatchAssetType = TEXT("MatchDefinition");
	USMAssetManager& AssetManager = USMAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssetsWithType(MatchAssetType);
	if (ensure(Handle.IsValid()))
	{
		Handle->WaitUntilComplete();
		TArray<UObject*> Matches;
		AssetManager.GetPrimaryAssetObjectList(MatchAssetType, Matches);
		return Matches;
	}
	return TArray<UObject*>();
}

USMMatchDefinition* USMMatchStatics::FindMatchDefinition(const FPrimaryAssetId& MatchAssetId)
{
	FSoftObjectPath Path = USMAssetManager::Get().GetPrimaryAssetPath(MatchAssetId);
	FSoftObjectPtr AssetPtr(Path);
	UObject* Asset = AssetPtr.LoadSynchronous();
	return Cast<USMMatchDefinition>(Asset);
}

USMMatchDefinition* USMMatchStatics::FindMatchDefinition(FName MatchDefinitionName)
{
	static const FPrimaryAssetType MatchAssetType = TEXT("MatchDefinition");
	return FindMatchDefinition(FPrimaryAssetId(MatchAssetType, MatchDefinitionName));
}
