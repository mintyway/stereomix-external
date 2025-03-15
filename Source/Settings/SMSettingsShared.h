// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Player/SMLocalPlayer.h"
#include "SMSettingsShared.generated.h"

UENUM(BlueprintType)
enum class EColorBlindMode : uint8
{
	Off,
	Deuteranope, // Deuteranope (green weak/blind)
	Protanope,   // Protanope (red weak/blind)
	Tritanope    // Tritanope(blue weak / bind)
};

UENUM(BlueprintType)
enum class ESMAllowBackgroundAudioSetting : uint8
{
	Off,
	AllSounds,

	Num UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESMGamepadSensitivity : uint8
{
	Invalid = 0 UMETA(Hidden),

	Slow UMETA(DisplayName = "01 - Slow"),
	SlowPlus UMETA(DisplayName = "02 - Slow+"),
	SlowPlusPlus UMETA(DisplayName = "03 - Slow++"),
	Normal UMETA(DisplayName = "04 - Normal"),
	NormalPlus UMETA(DisplayName = "05 - Normal+"),
	NormalPlusPlus UMETA(DisplayName = "06 - Normal++"),
	Fast UMETA(DisplayName = "07 - Fast"),
	FastPlus UMETA(DisplayName = "08 - Fast+"),
	FastPlusPlus UMETA(DisplayName = "09 - Fast++"),
	Insane UMETA(DisplayName = "10 - Insane"),

	Num UMETA(Hidden),
};

class USMLocalPlayer;

/**
 * 클라우드 등에 저장되는 유저 별 설정을 모아놓은 클래스입니다.
 */
UCLASS(MinimalAPI)
class USMSettingsShared : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	USMSettingsShared();

	virtual int32 GetLatestDataVersion() const override;

	bool IsDirty() const { return bIsDirty; }
	void ClearDirtyFlag() { bIsDirty = false; }

	static USMSettingsShared* CreateTemporarySettings(const USMLocalPlayer* LocalPlayer);

	/** 동기적으로 설정 정보를 가져옵니다. 유저가 로그인하기 전에는 올바르지 않은 정보를 반환합니다. */
	static USMSettingsShared* LoadOrCreateSettings(const USMLocalPlayer* LocalPlayer);

	DECLARE_DELEGATE_OneParam(FSMOnSettingsLoaded, USMSettingsShared* Settings);

	/** 비동기적으로 설정 정보를 가져옵니다. 유저가 로그인하기 전에는 올바르지 않은 정보를 반환합니다. */
	static bool AsyncLoadOrCreateSettings(const USMLocalPlayer* LocalPlayer, FSMOnSettingsLoaded OnSettingsLoaded);

	/** 디스크에 설정을 저장합니다. */
	void SaveSettings();

	/** 현재 설정을 적용합니다. */
	void ApplySettings();

	DECLARE_EVENT_OneParam(USMSettingsShared, FSMOnSettingChanged, USMSettingsShared* Settings);

	FSMOnSettingChanged OnSettingChangedEvent;

private:
	template<typename T>
	bool ChangeValueAndDirty(T& CurrentValue, const T& NewValue)
	{
		if (CurrentValue != NewValue)
		{
			CurrentValue = NewValue;
			bIsDirty = true;
			OnSettingChangedEvent.Broadcast(this);

			return true;
		}
		return false;
	}

	bool bIsDirty = false;


	//////////////////////////////////////////////////////////////////
	// Color Blind Options
public:
	UFUNCTION()
	EColorBlindMode GetColorBlindMode() const;
	UFUNCTION()
	void SetColorBlindMode(EColorBlindMode InMode);

	UFUNCTION()
	int32 GetColorBlindStrength() const;
	UFUNCTION()
	void SetColorBlindStrength(int32 InColorBlindStrength);

	void ApplyColorBlindSettings();

private:
	UPROPERTY()
	EColorBlindMode ColorBlindMode = EColorBlindMode::Off;

	UPROPERTY()
	int32 ColorBlindStrength = 10;


	//////////////////////////////////////////////////////////////////
	// Gamepad Vibration
public:
	UFUNCTION()
	bool GetForceFeedbackEnabled() const { return bForceFeedbackEnabled; }

	UFUNCTION()
	void SetForceFeedbackEnabled(const bool NewValue) { ChangeValueAndDirty(bForceFeedbackEnabled, NewValue); }

private:
	/** Is force feedback enabled when a controller is being used? */
	UPROPERTY()
	bool bForceFeedbackEnabled = true;


	//////////////////////////////////////////////////////////////////
	// Gamepad Deadzone
public:
	/** Getter for gamepad move stick dead zone value. */
	UFUNCTION()
	float GetGamepadMoveStickDeadZone() const { return GamepadMoveStickDeadZone; }

	/** Setter for gamepad move stick dead zone value. */
	UFUNCTION()
	void SetGamepadMoveStickDeadZone(const float NewValue) { ChangeValueAndDirty(GamepadMoveStickDeadZone, NewValue); }

	/** Getter for gamepad look stick dead zone value. */
	UFUNCTION()
	float GetGamepadLookStickDeadZone() const { return GamepadLookStickDeadZone; }

	/** Setter for gamepad look stick dead zone value. */
	UFUNCTION()
	void SetGamepadLookStickDeadZone(const float NewValue) { ChangeValueAndDirty(GamepadLookStickDeadZone, NewValue); }

private:
	/** Holds the gamepad move stick dead zone value. */
	UPROPERTY()
	float GamepadMoveStickDeadZone;

	/** Holds the gamepad look stick dead zone value. */
	UPROPERTY()
	float GamepadLookStickDeadZone;


	//////////////////////////////////////////////////////////////////
	// Gamepad Trigger Haptics
public:
	UFUNCTION()
	bool GetTriggerHapticsEnabled() const { return bTriggerHapticsEnabled; }

	UFUNCTION()
	void SetTriggerHapticsEnabled(const bool NewValue) { ChangeValueAndDirty(bTriggerHapticsEnabled, NewValue); }

	UFUNCTION()
	bool GetTriggerPullUsesHapticThreshold() const { return bTriggerPullUsesHapticThreshold; }

	UFUNCTION()
	void SetTriggerPullUsesHapticThreshold(const bool NewValue) { ChangeValueAndDirty(bTriggerPullUsesHapticThreshold, NewValue); }

	UFUNCTION()
	uint8 GetTriggerHapticStrength() const { return TriggerHapticStrength; }

	UFUNCTION()
	void SetTriggerHapticStrength(const uint8 NewValue) { ChangeValueAndDirty(TriggerHapticStrength, NewValue); }

	UFUNCTION()
	uint8 GetTriggerHapticStartPosition() const { return TriggerHapticStartPosition; }

	UFUNCTION()
	void SetTriggerHapticStartPosition(const uint8 NewValue) { ChangeValueAndDirty(TriggerHapticStartPosition, NewValue); }

private:
	/** Are trigger haptics enabled? */
	UPROPERTY()
	bool bTriggerHapticsEnabled = false;
	/** Does the game use the haptic feedback as its threshold for judging button presses? */
	UPROPERTY()
	bool bTriggerPullUsesHapticThreshold = true;
	/** The strength of the trigger haptic effects. */
	UPROPERTY()
	uint8 TriggerHapticStrength = 8;
	/** The start position of the trigger haptic effects */
	UPROPERTY()
	uint8 TriggerHapticStartPosition = 0;


	//////////////////////////////////////////////////////////////////
	// Shared Audio Settings
public:
	UFUNCTION()
	ESMAllowBackgroundAudioSetting GetAllowAudioInBackgroundSetting() const { return AllowAudioInBackground; }

	UFUNCTION()
	void SetAllowAudioInBackgroundSetting(ESMAllowBackgroundAudioSetting NewValue);

	void ApplyBackgroundAudioSettings();

private:
	UPROPERTY()
	ESMAllowBackgroundAudioSetting AllowAudioInBackground = ESMAllowBackgroundAudioSetting::Off;


	//////////////////////////////////////////////////////////////////
	// Culture / Language
public:
	/** Gets the pending culture */
	UFUNCTION()
	const FString& GetPendingCulture() const;

	/** Sets the pending culture to apply */
	UFUNCTION()
	void SetPendingCulture(const FString& NewCulture);

	UFUNCTION()
	void SetAndApplyCulture(const FString& NewCulture);
	
	// Called when the culture changes.
	void OnCultureChanged();

	/** Clears the pending culture to apply */
	void ClearPendingCulture();

	bool IsUsingDefaultCulture() const;

	void ResetToDefaultCulture();
	bool ShouldResetToDefaultCulture() const { return bResetToDefaultCulture; }

	void ApplyCultureSettings();
	void ResetCultureToCurrentSettings();

private:
	/** The pending culture to apply */
	UPROPERTY(Transient)
	FString PendingCulture;

	/* If true, resets the culture to default. */
	bool bResetToDefaultCulture = false;


	//////////////////////////////////////////////////////////////////
	// Mouse Sensitivity
public:
	UFUNCTION()
	double GetMouseSensitivityX() const { return MouseSensitivityX; }

	UFUNCTION()
	void SetMouseSensitivityX(double NewValue)
	{
		ChangeValueAndDirty(MouseSensitivityX, NewValue);
		ApplyInputSensitivity();
	}

	UFUNCTION()
	double GetMouseSensitivityY() const { return MouseSensitivityY; }

	UFUNCTION()
	void SetMouseSensitivityY(double NewValue)
	{
		ChangeValueAndDirty(MouseSensitivityY, NewValue);
		ApplyInputSensitivity();
	}

	UFUNCTION()
	double GetTargetingMultiplier() const { return TargetingMultiplier; }

	UFUNCTION()
	void SetTargetingMultiplier(double NewValue)
	{
		ChangeValueAndDirty(TargetingMultiplier, NewValue);
		ApplyInputSensitivity();
	}

	UFUNCTION()
	bool GetInvertVerticalAxis() const { return bInvertVerticalAxis; }

	UFUNCTION()
	void SetInvertVerticalAxis(bool NewValue)
	{
		ChangeValueAndDirty(bInvertVerticalAxis, NewValue);
		ApplyInputSensitivity();
	}

	UFUNCTION()
	bool GetInvertHorizontalAxis() const { return bInvertHorizontalAxis; }

	UFUNCTION()
	void SetInvertHorizontalAxis(bool NewValue)
	{
		ChangeValueAndDirty(bInvertHorizontalAxis, NewValue);
		ApplyInputSensitivity();
	}

private:
	/** Holds the mouse horizontal sensitivity */
	UPROPERTY()
	double MouseSensitivityX = 1.0;

	/** Holds the mouse vertical sensitivity */
	UPROPERTY()
	double MouseSensitivityY = 1.0;

	/** Multiplier applied while Aiming down sights. */
	UPROPERTY()
	double TargetingMultiplier = 0.5;

	/** If true then the vertical look axis should be inverted */
	UPROPERTY()
	bool bInvertVerticalAxis = false;

	/** If true then the horizontal look axis should be inverted */
	UPROPERTY()
	bool bInvertHorizontalAxis = false;


	//////////////////////////////////////////////////////////////////
	// Gamepad Sensitivity
public:
	UFUNCTION()
	ESMGamepadSensitivity GetGamepadLookSensitivityPreset() const { return GamepadLookSensitivityPreset; }

	UFUNCTION()
	void SetLookSensitivityPreset(ESMGamepadSensitivity NewValue)
	{
		ChangeValueAndDirty(GamepadLookSensitivityPreset, NewValue);
		ApplyInputSensitivity();
	}

	UFUNCTION()
	ESMGamepadSensitivity GetGamepadTargetingSensitivityPreset() const { return GamepadTargetingSensitivityPreset; }

	UFUNCTION()
	void SetGamepadTargetingSensitivityPreset(ESMGamepadSensitivity NewValue)
	{
		ChangeValueAndDirty(GamepadTargetingSensitivityPreset, NewValue);
		ApplyInputSensitivity();
	}

	void ApplyInputSensitivity();

private:
	UPROPERTY()
	ESMGamepadSensitivity GamepadLookSensitivityPreset = ESMGamepadSensitivity::Normal;
	UPROPERTY()
	ESMGamepadSensitivity GamepadTargetingSensitivityPreset = ESMGamepadSensitivity::Normal;
};
