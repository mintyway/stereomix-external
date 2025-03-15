// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingRegistry.h"
#include "Player/SMLocalPlayer.h"

#include "SMGameSettingRegistry.generated.h"

class USMLocalPlayer;
DECLARE_LOG_CATEGORY_EXTERN(LogStereoMixGameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName) \
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({ \
		GET_FUNCTION_NAME_STRING_CHECKED(USMLocalPlayer, GetSharedSettings), \
		GET_FUNCTION_NAME_STRING_CHECKED(USMSettingsShared, FunctionOrPropertyName) \
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName) \
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({ \
		GET_FUNCTION_NAME_STRING_CHECKED(USMLocalPlayer, GetLocalSettings), \
		GET_FUNCTION_NAME_STRING_CHECKED(USMSettingsLocal, FunctionOrPropertyName) \
	}))


UCLASS(MinimalAPI)
class USMGameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()

public:
	USMGameSettingRegistry();

	static USMGameSettingRegistry* Get(USMLocalPlayer* InLocalPlayer);

	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	static UGameSettingCollection* InitializeVideoSettings(USMLocalPlayer* InLocalPlayer);
	static void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, USMLocalPlayer* InLocalPlayer);
	static void AddPerformanceStatPage(UGameSettingCollection* PerfStatsOuterCategory, USMLocalPlayer* InLocalPlayer);

	static UGameSettingCollection* InitializeAudioSettings(USMLocalPlayer* InLocalPlayer);

	static UGameSettingCollection* InitializeGameplaySettings(USMLocalPlayer* InLocalPlayer);

	static UGameSettingCollection* InitializeMouseAndKeyboardSettings(USMLocalPlayer* InLocalPlayer);

	static UGameSettingCollection* InitializeGamepadSettings(USMLocalPlayer* InLocalPlayer);

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> VideoSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> AudioSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GameplaySettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> MouseAndKeyboardSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GamepadSettings;
};
