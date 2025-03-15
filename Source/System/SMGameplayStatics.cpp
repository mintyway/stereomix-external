// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayStatics.h"

#include "GeneralProjectSettings.h"

FString USMGameplayStatics::GetProjectVersion()
{
	const UGeneralProjectSettings* GeneralProjectSettings = GetDefault<UGeneralProjectSettings>();
	return GeneralProjectSettings->ProjectVersion;
}

FString USMGameplayStatics::GetBuildVersion()
{
	return FApp::GetBuildVersion();
}
