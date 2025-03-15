// Copyright Studio Surround. All Rights Reserved.


#include "StereoMixEngine.h"

#include "StereoMixLog.h"
#include "Misc/ConfigContext.h"

class IEngineLoop;

UStereoMixEngine::UStereoMixEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UStereoMixEngine::Init(IEngineLoop* InEngineLoop)
{
	FString CustomConfig = FConfigCacheIni::GetCustomConfigString();
	if (CustomConfig.IsEmpty())
	{
		UE_LOG(LogStereoMix, Warning, TEXT("CustomConfig is not set."));
	}
	else
	{
		UE_LOG(LogStereoMix, Warning, TEXT("CustomConfig: %s"), *CustomConfig);
	}
	Super::Init(InEngineLoop);
}
