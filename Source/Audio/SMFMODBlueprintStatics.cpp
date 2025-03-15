// Copyright Surround, Inc. All Rights Reserved.


#include "SMFMODBlueprintStatics.h"

#include "FMODBus.h"
#include "FMODVCA.h"

float USMFMODBlueprintStatics::BusGetVolume(UFMODBus* Bus)
{
	FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (StudioSystem != nullptr && IsValid(Bus))
	{
		FMOD::Studio::ID Guid = FMODUtils::ConvertGuid(Bus->AssetGuid);
		FMOD::Studio::Bus* BusPtr = nullptr;
		FMOD_RESULT Result = StudioSystem->getBusByID(&Guid, &BusPtr);
		if (Result == FMOD_OK && BusPtr != nullptr)
		{
			float Volume;
			BusPtr->getVolume(&Volume);
			return Volume;
		}
	}

	return 0.0f;
}

float USMFMODBlueprintStatics::VCAGetVolume(UFMODVCA* VCA)
{
	FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (StudioSystem != nullptr && IsValid(VCA))
	{
		FMOD::Studio::ID Guid = FMODUtils::ConvertGuid(VCA->AssetGuid);
		FMOD::Studio::VCA* VCAPtr = nullptr;
		FMOD_RESULT Result = StudioSystem->getVCAByID(&Guid, &VCAPtr);
		if (Result == FMOD_OK && VCAPtr != nullptr)
		{
			float Volume;
			VCAPtr->getVolume(&Volume);
			return Volume;
		}
	}

	return 0.0f;
}

FString USMFMODBlueprintStatics::GetCurrentDriverName()
{
	const int32 CurrentDriverIndex = GetCurrentDriverIndex();

	FString DriverName;
	FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (StudioSystem != nullptr)
	{
		FMOD::System* LowLevelSystem = nullptr;
		verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));

		char DriverNameUTF8[256] = {};
		verifyfmod(LowLevelSystem->getDriverInfo(CurrentDriverIndex, DriverNameUTF8, sizeof(DriverNameUTF8), nullptr, nullptr, nullptr, nullptr));
		DriverName = UTF8_TO_TCHAR(DriverNameUTF8);
	}
	return DriverName;
}

int32 USMFMODBlueprintStatics::GetCurrentDriverIndex()
{
	int32 CurrentDriverIndex = 0;
	FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (StudioSystem != nullptr)
	{
		FMOD::System* LowLevelSystem = nullptr;
		verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));
		verifyfmod(LowLevelSystem->getDriver(&CurrentDriverIndex))
	}
	return CurrentDriverIndex;
}
