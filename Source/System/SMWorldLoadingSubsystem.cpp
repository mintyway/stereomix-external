// Copyright Studio Surround. All Rights Reserved.


#include "SMWorldLoadingSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMWorldLoadingSubsystem)

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixWorldLoading, Log, All);

ETickableTickType USMWorldLoadingSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool USMWorldLoadingSubsystem::IsTickable() const
{
	return !bLevelLoaded;
}

TStatId USMWorldLoadingSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USMWorldLoadingSubsystem, STATGROUP_Tickables);
}

void USMWorldLoadingSubsystem::UpdateStreamingState()
{
	if (bLevelLoaded)
	{
		return;
	}

	// 스트리밍 레벨 로딩 체크
	const TArray<ULevelStreaming*>& StreamingLevels = GetWorld()->GetStreamingLevels();
	TArray<const ULevelStreaming*> PendingStreamingLevels;

	for (const ULevelStreaming* StreamingLevel : StreamingLevels)
	{
		if (StreamingLevel && StreamingLevel->IsStreamingStatePending())
		{
			PendingStreamingLevels.Add(StreamingLevel);
		}
	}
	if (PendingStreamingLevels.Num() != 0)
	{
		UE_LOG(LogStereoMixWorldLoading, Verbose, TEXT("Streaming levels are pending. (%d/%d)"), StreamingLevels.Num() - PendingStreamingLevels.Num(), StreamingLevels.Num());

		UE_LOG(LogStereoMixWorldLoading, VeryVerbose, TEXT("Pending streaming levels: %s"),
			*FString::JoinBy(PendingStreamingLevels, TEXT(", "), [](const ULevelStreaming* StreamingLevel) {
				return StreamingLevel ? StreamingLevel->GetWorldAssetPackageName() : TEXT("null");
				}
			));

		return;
	}

	// 레벨 인스턴스 로딩은 아직 필요가 없어서 주석처리함.
	// 나중에 필요하다면 주석해제 후 사용

	// if (ULevelInstanceSubsystem* LevelInstanceSubsystem = GetWorld()->GetSubsystem<ULevelInstanceSubsystem>())
	// {
	// 	TArray<ULevel*> Levels = GetWorld()->GetLevels();
	// 	if (Levels.Num() == 0)
	// 	{
	// 		UE_LOG(LogStereoMixWorldLoading, Verbose, TEXT("No levels."));
	// 		return;
	// 	}
	// 	for (ULevel* Level : Levels)
	// 	{
	// 		if (Level)
	// 		{
	// 			// UE_LOG(LogStereoMixWorldLoading, Verbose, TEXT("Level: %s"), *Level->GetPathName());
	// 			if (ILevelInstanceInterface* LevelInstance = LevelInstanceSubsystem->GetOwningLevelInstance(Level))
	// 			{
	// 				if (LevelInstanceSubsystem->IsLoading(LevelInstance))
	// 				{
	// 					UE_LOG(LogStereoMixWorldLoading, Verbose, TEXT("LevelInstance is loading. (%s)"), *Level->GetPathName());
	// 					return;
	// 				}
	// 			}
	// 			else
	// 			{
	// 				UE_LOG(LogStereoMixWorldLoading, Verbose, TEXT("LevelInstance is not found. (%s)"), *Level->GetPathName());
	// 			}
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogStereoMixWorldLoading, Verbose, TEXT("Level is not found."));
	// 		}
	// 	}

	UE_LOG(LogStereoMixWorldLoading, Warning, TEXT("Level is loaded."));
	bLevelLoaded = true;
	OnLevelLoadedEvent.Broadcast();
	K2_OnLevelLoadedEvent.Broadcast();
}
