// Copyright Studio Surround. All Rights Reserved.


#include "SMAssetManager.h"

#include "StereoMixLog.h"

USMAssetManager::USMAssetManager()
{
}

USMAssetManager& USMAssetManager::Get()
{
	check(GEngine);
	if (USMAssetManager* Singleton = Cast<USMAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogStereoMix, Fatal, TEXT("Invalid AssetManager in GameEngine."));
	return *NewObject<USMAssetManager>();
}

FName USMAssetManager::GetAssetName(const FPrimaryAssetId& PrimaryAssetId)
{
	FAssetData AssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(PrimaryAssetId, /*out*/ AssetData))
	{
		return AssetData.PackageName;
	}
	return NAME_None;
}

FString USMAssetManager::GetAssetClassPath(const FPrimaryAssetId& PrimaryAssetId)
{
	FAssetData AssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(PrimaryAssetId, /*out*/ AssetData))
	{
		// TODO: 이 하드코딩된 방법 말고 다른 방법 있는지 확인
		return AssetData.GetObjectPathString() + "_C";
	}
	return FString();
}

void USMAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogStereoMix, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogStereoMix, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogStereoMix, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogStereoMix, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

UObject* USMAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool USMAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void USMAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void USMAssetManager::StartInitialLoading()
{
	// SCOPED_BOOT_TIMING("USMAssetManager::StartInitialLoading");
	Super::StartInitialLoading();
}

#if WITH_EDITOR
void USMAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);
	{
		FScopedSlowTask SlowTask(0, FText::FromString(TEXT("Loading PIE Data")));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		// const ULyraGameData& LocalGameDataCommon = GetGameData();

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}
#endif

UPrimaryDataAsset* USMAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(FText::FromString(TEXT("Loading GameData {0}")), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogStereoMix, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogStereoMix, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}
