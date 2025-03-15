// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "SMSettingValueDiscreteDynamic_AudioOutputDevice.generated.h"


struct FAudioOutputDeviceInfo;
struct FSwapAudioOutputResult;
enum class EAudioDeviceChangedRole : uint8;

UCLASS(MinimalAPI)
class USMSettingValueDiscreteDynamic_AudioOutputDevice : public UGameSettingValueDiscreteDynamic
{
	GENERATED_BODY()

protected:
	virtual void OnInitialized() override;
};
