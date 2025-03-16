﻿// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "SMLevelChanger.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMLevelChanger : public AActor
{
	GENERATED_BODY()

public:
	ASMLevelChanger();

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	/** 레벨 전환 시작을 지연시키기 위한 시간입니다. */
	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float StartDelay = 0.0f;

	/** 레벨 전환 주기입니다. */
	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "1"))
	float LevelSwitchInterval = 10.0f;

	/** 오브젝트가 생성되기 전에 사전 이펙트가 존재하는데 이를 위한 시간입니다. (프리 로드 시간) */
	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float ObstacleSpawnEffectDuration = 3.0f;

	/** 동시에 활성화 가능한 레벨 개수입니다. 프리 로드 레벨은 포함되지 않습니다. */
	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "1"))
	int32 ActiveLevelCount = 1;

	UPROPERTY(EditAnywhere, Category = "Design")
	TArray<FName> SubLevels;

	TArray<FName> ActiveSubLevels;

	TArray<FName> PreviouslyActiveSubLevels;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowActiveEffect();

	void StartLevelSwitchInterval();

	void SetRandomSubLevel();

	bool IsLevelActive(FName SubLevel) const;

	bool IsLevelPreviouslyActive(FName SubLevel) const;

	void MarkLevelAsActive(FName SubLevel);

	void SetLevelVisibility(FName SubLevel, bool bVisibility);
};
