﻿// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "SMCountdownTimerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTCountdownimerDelegate);

UCLASS(ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class STEREOMIX_API USMCountdownTimerComponent : public UActorComponent
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;

	// TODO: float으로 바꾸고 tick interval을 변경 가능하도록 수정하기

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = OnRep_RemainingTime, meta = (AllowPrivateAccess = true))
	int32 RemainingTime;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, meta = (AllowPrivateAccess = true))
	int32 InitTime;

	UFUNCTION()
	void OnRep_RemainingTime() const;

public:
	UPROPERTY(BlueprintAssignable)
	FTCountdownimerDelegate OnCountdownTick;

	UPROPERTY(BlueprintAssignable)
	FTCountdownimerDelegate OnCountdownFinished;

	UPROPERTY(BlueprintAssignable)
	FTCountdownimerDelegate OnCountdownCancelled;

	USMCountdownTimerComponent();

	int32 GetInitTime() const { return InitTime; }

	int32 GetRemainingTime() const { return RemainingTime; }

	bool IsRunning() const { return TimerHandle.IsValid(); }

	void StartCountdown(int32 Seconds);

	void CancelCountdown();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PerformCountdownTick();
};
