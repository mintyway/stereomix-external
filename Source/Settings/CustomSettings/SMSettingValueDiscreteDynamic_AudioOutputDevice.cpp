// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingValueDiscreteDynamic_AudioOutputDevice.h"

#include "FMODBlueprintStatics.h"
#include "Audio/SMFMODBlueprintStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingValueDiscreteDynamic_AudioOutputDevice)

#define LOCTABLE_ID "Settings"

void USMSettingValueDiscreteDynamic_AudioOutputDevice::OnInitialized()
{
	Super::OnInitialized();

	TArray<FString> Drivers = USMFMODBlueprintStatics::GetOutputDrivers();
	const int32 NewSize = Drivers.Num();
	OptionValues.Reset(NewSize);
	OptionDisplayTexts.Reset(NewSize);

	if (NewSize > 0)
	{
		for (const FString& Driver : Drivers)
		{
			AddDynamicOption(Driver, FText::FromString(Driver));
		}

		SetDefaultValueFromString(Drivers[0]);
		RefreshEditableState();
	}
}

#undef LOCTABLE_ID
