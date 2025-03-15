// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "SMGameplayPopWidget.h"
#include "SMGameplayPopSettings.generated.h"

/**
 * StereoMix Gameplay Pop Settings
 */
UCLASS(Config=Game, DefaultConfig, DisplayName="StereoMix Gameplay Pop Settings")
class STEREOMIX_API USMGameplayPopSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	// 초기 풀 사이즈
	UPROPERTY(EditAnywhere, Config, Category = "Gameplay Pop")
	int32 InitialPoolSize = 20;

	// 틱 당 최대 활성화 팝 수
	UPROPERTY(EditAnywhere, Config, Category = "Gameplay Pop")
	int32 MaxActivatePopsPerTick = 5;

	// TSoftClassPtr쓰면 스탠드얼론에서 클래스를 제대로 가져오지 못해서 일단 TSubclassOf로 사용

	UPROPERTY(EditAnywhere, Config, Category = "Gameplay Pop")
	TSubclassOf<USMGameplayPopWidget> DefaultDamagePopWidget;

	UPROPERTY(EditAnywhere, Config, Category = "Gameplay Pop")
	TSubclassOf<USMGameplayPopWidget> DefaultItemPopWidget;

	UPROPERTY(EditAnywhere, Config, Category = "Gameplay Pop")
	TSubclassOf<USMGameplayPopWidget> DefaultBuffPopWidget;

	UPROPERTY(EditAnywhere, Config, Category = "Gameplay Pop")
	TSubclassOf<USMGameplayPopWidget> DefaultTilePopWidget;
};
