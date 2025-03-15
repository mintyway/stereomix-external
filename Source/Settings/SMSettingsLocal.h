// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FMODBlueprintStatics.h"
#include "SMSettingsLocal.generated.h"


class UPlayerMappableInputConfig;
class USoundControlBus;
class USoundControlBusMix;
class UFMODVCA;
class UFMODBus;
enum class ESMDisplayablePerformanceStat : uint8;
enum class ESMStatDisplayMode : uint8;

USTRUCT()
struct FSMScalabilitySnapshot
{
	GENERATED_BODY()

	FSMScalabilitySnapshot();

	Scalability::FQualityLevels Qualities;
	bool bActive = false;
	bool bHasOverrides = false;
};

UCLASS(MinimalAPI)
class USMSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()

public:
	USMSettingsLocal();

	static USMSettingsLocal* Get();

	virtual void BeginDestroy() override;
	virtual void SetToDefaults() override;
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ConfirmVideoMode() override;
	virtual float GetEffectiveFrameRateLimit() override;
	virtual void ResetToCurrentSettings() override;
	virtual void ApplyNonResolutionSettings() override;
	virtual int32 GetOverallScalabilityLevel() const override;
	virtual void SetOverallScalabilityLevel(int32 Value) override;

	void ApplyScalabilitySettings();

private:
	void OnAppActivationStateChanged(bool bIsActive);
	void ReapplyThingsDueToPossibleDeviceProfileChange();

	FDelegateHandle OnApplicationActivationStateChangedHandle;


	//////////////////////////////////////////////////////////////////////////
	// Frontend State
public:
	void SetShouldUseFrontendPerformanceSettings(bool bInFrontEnd);

protected:
	bool ShouldUseFrontendPerformanceSettings() const;

private:
	bool bInFrontEndForPerformancePurposes = false;


	//////////////////////////////////////////////////////////////////////////
	// Performance Stats
public:
	ESMStatDisplayMode GetPerfStatDisplayState(ESMDisplayablePerformanceStat Stat) const;
	void SetPerfStatDisplayState(ESMDisplayablePerformanceStat Stat, ESMStatDisplayMode DisplayMode);

	DECLARE_EVENT(USMSettingsLocal, FPerfStatSettingsChanged);

	FPerfStatSettingsChanged& OnPerfStatSettingsChanged() { return PerfStatSettingsChangedEvent; }

private:
	UPROPERTY(Config)
	TMap<ESMDisplayablePerformanceStat, ESMStatDisplayMode> DisplayStatList;

	FPerfStatSettingsChanged PerfStatSettingsChangedEvent;


	//////////////////////////////////////////////////////////////////////////
	// Brightness / Gamma
public:
	UFUNCTION()
	float GetDisplayGamma() const;
	UFUNCTION()
	void SetDisplayGamma(float InGamma);

private:
	void ApplyDisplayGamma();

	UPROPERTY(Config)
	float DisplayGamma = 2.2;


	//////////////////////////////////////////////////////////////////////////
	// Display
public:
	UFUNCTION()
	float GetFrameRateLimit_Always() const;
	UFUNCTION()
	void SetFrameRateLimit_Always(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_InMenu() const;
	UFUNCTION()
	void SetFrameRateLimit_InMenu(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_WhenBackground() const;
	UFUNCTION()
	void SetFrameRateLimit_WhenBackground(float NewLimitFPS);

protected:
	void UpdateEffectiveFrameRateLimit();

private:
	UPROPERTY(Config)
	float FrameRateLimit_InMenu;

	UPROPERTY(Config)
	float FrameRateLimit_WhenBackground;

	FSMScalabilitySnapshot DeviceDefaultScalabilitySettings;

	bool bSettingOverallQualityGuard = false;


	//////////////////////////////////////////////////////////////////////////
	// Display - Quality presets
public:
	UFUNCTION()
	FString GetDesiredDeviceProfileQualitySuffix() const;

	UFUNCTION()
	void SetDesiredDeviceProfileQualitySuffix(const FString& InDesiredSuffix);

	int32 GetMaxSupportedOverallQualityLevel() const;

	/** Returns true if this platform can run the auto benchmark */
	UFUNCTION(BlueprintCallable, Category = Settings)
	bool CanRunAutoBenchmark() const;

	/** Returns true if this user should run the auto benchmark as it has never been run */
	UFUNCTION(BlueprintCallable, Category = Settings)
	bool ShouldRunAutoBenchmarkAtStartup() const;

	/** Run the auto benchmark, optionally saving right away */
	UFUNCTION(BlueprintCallable, Category = Settings)
	void RunAutoBenchmark(bool bSaveImmediately);

protected:
	/** Updates device profiles, FPS mode etc for the current game mode */
	void UpdateGameModeDeviceProfileAndFps();

	void UpdateConsoleFramePacing();
	void UpdateDesktopFramePacing();

	void UpdateDynamicResFrameTime(float TargetFPS);

private:
	UPROPERTY(Transient)
	FString DesiredUserChosenDeviceProfileSuffix;

	UPROPERTY(Transient)
	FString CurrentAppliedDeviceProfileOverrideSuffix;

	UPROPERTY(config)
	FString UserChosenDeviceProfileSuffix;


	//////////////////////////////////////////////////////////////////////////
	// Audio - Volume
public:
	UFUNCTION()
	float GetOverallVolume() const;
	UFUNCTION()
	void SetOverallVolume(float InVolume);

	UFUNCTION()
	float GetMusicVolume() const;
	UFUNCTION()
	void SetMusicVolume(float InVolume);

	UFUNCTION()
	float GetSoundFXVolume() const;
	UFUNCTION()
	void SetSoundFXVolume(float InVolume);

	UFUNCTION()
	float GetVoiceChatVolume() const;
	UFUNCTION()
	void SetVoiceChatVolume(float InVolume);

private:
	void SetVolumeForControlBus(USoundControlBus* InSoundControlBus, float InVolume);
	void SetVolumeForVCA(UFMODVCA* TargetVCA, float InVolume);
	
	void LoadUserControlBusMix(); 
	void LoadFMOD();

	UPROPERTY(Config)
	float OverallVolume = 1.0f;
	UPROPERTY(Config)
	float MusicVolume = 1.0f;
	UPROPERTY(Config)
	float SoundFXVolume = 1.0f;
	UPROPERTY(Config)
	float VoiceChatVolume = 1.0f;

	UPROPERTY(Transient)
	bool bSoundControlBusMixLoaded = false;

	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USoundControlBus>> ControlBusMap;

	UPROPERTY(Transient)
	TObjectPtr<USoundControlBusMix> ControlBusMix = nullptr;
	
	UPROPERTY(Transient)
	bool bFMODLoaded = false;

	UPROPERTY(Transient)
	TObjectPtr<UFMODVCA> FMOD_MasterVCA;
	UPROPERTY(Transient)
	TObjectPtr<UFMODVCA> FMOD_MusicVCA;
	UPROPERTY(Transient)
	TObjectPtr<UFMODVCA> FMOD_SoundFxVCA;

	//////////////////////////////////////////////////////////////////////////
	// Audio - Device
public:
	DECLARE_EVENT_OneParam(USMSettingsLocal, FAudioDeviceChanged, const FString& DeviceId);

	FAudioDeviceChanged& OnAudioDeviceChanged() { return OnAudioDeviceChangedEvent; }

	UFUNCTION()
	FString GetAudioOutputDeviceId() const { return AudioOutputDeviceId; }

	UFUNCTION()
	void SetAudioOutputDeviceId(const FString& InAudioOutputDeviceId);

private:
	FAudioDeviceChanged OnAudioDeviceChangedEvent;

	UPROPERTY(Config)
	FString AudioOutputDeviceId;


	//////////////////////////////////////////////////////////////////////////
	// Controls
public:
	// Sets the controller representation to use, a single platform might support multiple kinds of controllers.  For
	// example, Win64 games could be played with both an XBox or Playstation controller.
	UFUNCTION()
	void SetControllerPlatform(const FName InControllerPlatform);
	UFUNCTION()
	FName GetControllerPlatform() const;

private:
	/**
	 * The name of the controller the player is using.  This is maps to the name of a UCommonInputBaseControllerData
	 * that is available on this current platform.  The gamepad data are registered per platform, you'll find them
	 * in <Platform>Game.ini files listed under +ControllerData=...
	 */
	UPROPERTY(Config)
	FName ControllerPlatform;

	UPROPERTY(Config)
	FName ControllerPreset = TEXT("Default");

	/** The name of the current input config that the user has selected. */
	UPROPERTY(Config)
	FName InputConfigName = TEXT("Default");


	//////////////////////////////////////////////////////////////////////////
	// Replays
public:
	UFUNCTION()
	bool ShouldAutoRecordReplays() const { return bShouldAutoRecordReplays; }

	UFUNCTION()
	void SetShouldAutoRecordReplays(bool bEnabled) { bShouldAutoRecordReplays = bEnabled; }

	UFUNCTION()
	int32 GetNumberOfReplaysToKeep() const { return NumberOfReplaysToKeep; }

	UFUNCTION()
	void SetNumberOfReplaysToKeep(int32 InNumberOfReplays) { NumberOfReplaysToKeep = InNumberOfReplays; }

private:
	UPROPERTY(Config)
	bool bShouldAutoRecordReplays = false;

	UPROPERTY(Config)
	int32 NumberOfReplaysToKeep = 5;
};
