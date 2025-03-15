// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"
#include "Components/GameStateComponent.h"
#include "SMBackgroundMusicComponent.generated.h"


UCLASS(Abstract)
class STEREOMIX_API USMBackgroundMusicComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	static USMBackgroundMusicComponent* Get(const UObject* WorldContextObject);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsPlaying() const;
	void Play();
	void Stop();

protected:
	void OnBackgroundMusicLoaded();
	
	UPROPERTY(EditDefaultsOnly, Category = "Background Music")
	TSoftObjectPtr<UFMODEvent> BackgroundMusic;

	// true인 경우 SMGameState의 IsReadyToPlay가 true일 때 시작합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Background Music")
	bool bStartAfterGameReadyToPlay = true;

private:
	UPROPERTY(Transient)
	TObjectPtr<UFMODAudioComponent> MusicComponent;
	
	FFMODEventInstance MusicEventInstance;
};
