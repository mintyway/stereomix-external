// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "ChartCreation.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMPerformanceStatSubsystem.generated.h"

enum class ESMDisplayablePerformanceStat : uint8;
struct FFrame;
class FSubsystemCollectionBase;
class USMPerformanceStatSubsystem;
class UObject;

struct FSMPerformanceStatCache : public IPerformanceDataConsumer
{
	FSMPerformanceStatCache(USMPerformanceStatSubsystem* InSubsystem)
		: MySubsystem(InSubsystem)
	{
	}

	virtual void StartCharting() override;
	virtual void ProcessFrame(const FFrameData& FrameData) override;
	virtual void StopCharting() override;

	double GetCachedStat(ESMDisplayablePerformanceStat Stat) const;

protected:
	FFrameData CachedData;
	USMPerformanceStatSubsystem* MySubsystem;

	float CachedServerFPS = 0.0f;
	float CachedPingMS = 0.0f;
	float CachedPacketLossIncomingPercent = 0.0f;
	float CachedPacketLossOutgoingPercent = 0.0f;
	float CachedPacketRateIncoming = 0.0f;
	float CachedPacketRateOutgoing = 0.0f;
	float CachedPacketSizeIncoming = 0.0f;
	float CachedPacketSizeOutgoing = 0.0f;
};

/**
 * 캐싱된 성능 통계를 가져올 수 있는 서브시스템입니다.
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMPerformanceStatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	double GetCachedStat(ESMDisplayablePerformanceStat Stat) const;

protected:
	TSharedPtr<FSMPerformanceStatCache> Tracker;
};
