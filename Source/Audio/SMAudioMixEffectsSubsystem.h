// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SMAudioMixEffectsSubsystem.generated.h"


class USoundControlBusMix;
class USoundControlBus;

USTRUCT()
struct FSMAudioSubmixEffectsChain
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<USoundSubmix> Submix = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USoundEffectSubmixPreset>> SubmixEffectChain;
};

UCLASS()
class STEREOMIX_API USMAudioMixEffectsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	void OnLoadingScreenStatusChanged(bool bShowingLoadingScreen);
	void ApplyOrRemoveLoadingScreenMix(bool bWantsLoadingScreenMix);
	
	// Default Sound Control Bus Mix retrieved from the Lyra Audio Settings
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBusMix> DefaultBaseMix = nullptr;

	// Loading Screen Sound Control Bus Mix retrieved from the Lyra Audio Settings
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBusMix> LoadingScreenMix = nullptr;

	// User Sound Control Bus Mix retrieved from the Lyra Audio Settings
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBusMix> UserMix = nullptr;

	// Overall Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBus> OverallControlBus = nullptr;

	// Music Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBus> MusicControlBus = nullptr;

	// SoundFX Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBus> SoundFXControlBus = nullptr;

	// Dialogue Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBus> DialogueControlBus = nullptr;

	// VoiceChat Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	TObjectPtr<USoundControlBus> VoiceChatControlBus = nullptr;

	// Submix Effect Chain Overrides to apply when HDR Audio is turned on
	UPROPERTY(Transient)
	TArray<FSMAudioSubmixEffectsChain> HDRSubmixEffectChain;

	// Submix Effect hain Overrides to apply when HDR Audio is turned off
	UPROPERTY(Transient)
	TArray<FSMAudioSubmixEffectsChain> LDRSubmixEffectChain;

	bool bAppliedLoadingScreenMix = false;
};
