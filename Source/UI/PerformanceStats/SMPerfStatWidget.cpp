// Copyright Studio Surround. All Rights Reserved.


#include "SMPerfStatWidget.h"

#include "Performance/SMPerformanceStatSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPerfStatWidget)

double USMPerfStatWidget::FetchStatValue()
{
	if (!CachedStatSubsystem)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<USMPerformanceStatSubsystem>();
			}
		}
	}

	if (CachedStatSubsystem)
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	return 0.0;
}
