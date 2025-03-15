// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GameplayTagContainer.h"
#include "LevelSequence.h"
#include "Data/SMTeam.h"
#include "SMWorldSettings.generated.h"

UENUM(BlueprintType, DisplayName = "Developer PIE Net Mode")
enum class ESMDeveloperPIENetMode : uint8
{
	Standalone,
	ListenServer,
	Client
};

/**
 * 
 */
UCLASS(DisplayName = "StereoMix World Settings")
class STEREOMIX_API ASMWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	ASMWorldSettings();

	UFUNCTION(BlueprintCallable, DisplayName = "Get World Display Name", Category = "StereoMix|WorldSettings", meta=(WorldContext="WorldContextObject"))
	static FText GetDisplayName(UObject* WorldContextObject);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

#if WITH_EDITORONLY_DATA
	// 설정 시 에디터에서 PIE를 실행했을 때 Standalone 모드로 강제 전환됩니다.
	UPROPERTY(EditDefaultsOnly, Category=PIE)
	uint8 bForceUseNetMode : 1 = false;

	UPROPERTY(EditDefaultsOnly, Category=PIE, meta = (EditCondition = "bForceUseNetMode"))
	ESMDeveloperPIENetMode NetModeOverride;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "World")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Round|LevelSequence")
	TMap<ESMTeam, TSoftObjectPtr<ULevelSequence>> RoundCountdownLevelSequence;

	UPROPERTY(EditDefaultsOnly, Category = "Round|LevelSequence")
	TSoftObjectPtr<ULevelSequence> RoundStartLevelSequence;

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTagContainer GameplayTags;
};
