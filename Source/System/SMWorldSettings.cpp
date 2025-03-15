// Copyright Studio Surround. All Rights Reserved.


#include "SMWorldSettings.h"

#include "StereoMixLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMWorldSettings)

ASMWorldSettings::ASMWorldSettings()
{
#if WITH_EDITORONLY_DATA
	bForceUseNetMode = false;
	NetModeOverride = ESMDeveloperPIENetMode::Standalone;
#endif
}

FText ASMWorldSettings::GetDisplayName(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		if (ASMWorldSettings* WorldSettings = CastChecked<ASMWorldSettings>(World->GetWorldSettings()))
		{
			return WorldSettings->DisplayName;
		}
	}
	UE_LOG(LogStereoMix, Error, TEXT("Failed to get the world settings."));
	return FText::GetEmpty();
}

#if WITH_EDITOR
void ASMWorldSettings::CheckForErrors()
{
	Super::CheckForErrors();

	// 맵 에러 검출 로직 구현
}
#endif
