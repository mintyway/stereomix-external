// Copyright Studio Surround. All Rights Reserved.


#include "SMPerformanceStatSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "SMPerformanceStatTypes.h"
#include "Games/SMGameState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPerformanceStatSubsystem)

void FSMPerformanceStatCache::StartCharting()
{
}

void FSMPerformanceStatCache::ProcessFrame(const FFrameData& FrameData)
{
	CachedData = FrameData;
	CachedServerFPS = 0.0f;
	CachedPingMS = 0.0f;
	CachedPacketLossIncomingPercent = 0.0f;
	CachedPacketLossOutgoingPercent = 0.0f;
	CachedPacketRateIncoming = 0.0f;
	CachedPacketRateOutgoing = 0.0f;
	CachedPacketSizeIncoming = 0.0f;
	CachedPacketSizeOutgoing = 0.0f;

	if (UWorld* World = MySubsystem->GetGameInstance()->GetWorld())
	{
		if (const ASMGameState* GameState = World->GetGameState<ASMGameState>())
		{
			CachedServerFPS = GameState->GetServerFPS();
		}

		if (APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World))
		{
			if (APlayerState* PS = LocalPC->GetPlayerState<APlayerState>())
			{
				CachedPingMS = PS->GetPingInMilliseconds();
			}

			if (UNetConnection* NetConnection = LocalPC->GetNetConnection())
			{
				const UNetConnection::FNetConnectionPacketLoss& InLoss = NetConnection->GetInLossPercentage();
				CachedPacketLossIncomingPercent = InLoss.GetAvgLossPercentage();
				const UNetConnection::FNetConnectionPacketLoss& OutLoss = NetConnection->GetOutLossPercentage();
				CachedPacketLossOutgoingPercent = OutLoss.GetAvgLossPercentage();

				CachedPacketRateIncoming = NetConnection->InPacketsPerSecond;
				CachedPacketRateOutgoing = NetConnection->OutPacketsPerSecond;

				CachedPacketSizeIncoming = (NetConnection->InPacketsPerSecond != 0) ? NetConnection->InBytesPerSecond / static_cast<float>(NetConnection->InPacketsPerSecond) : 0.0f;
				CachedPacketSizeOutgoing = (NetConnection->OutPacketsPerSecond != 0) ? NetConnection->OutBytesPerSecond / static_cast<float>(NetConnection->OutPacketsPerSecond) : 0.0f;
			}
		}
	}
}

void FSMPerformanceStatCache::StopCharting()
{
}

double FSMPerformanceStatCache::GetCachedStat(ESMDisplayablePerformanceStat Stat) const
{
	static_assert(static_cast<int32>(ESMDisplayablePerformanceStat::Count) == 15, "Need to update this function to deal with new performance stats");
	switch (Stat)
	{
		case ESMDisplayablePerformanceStat::ClientFPS:
			return CachedData.TrueDeltaSeconds != 0.0 ? 1.0 / CachedData.TrueDeltaSeconds : 0.0;

		case ESMDisplayablePerformanceStat::ServerFPS:
			return CachedServerFPS;

		case ESMDisplayablePerformanceStat::IdleTime:
			return CachedData.IdleSeconds;

		case ESMDisplayablePerformanceStat::FrameTime:
			return CachedData.TrueDeltaSeconds;

		case ESMDisplayablePerformanceStat::FrameTime_GameThread:
			return CachedData.GameThreadTimeSeconds;

		case ESMDisplayablePerformanceStat::FrameTime_RenderThread:
			return CachedData.RenderThreadTimeSeconds;

		case ESMDisplayablePerformanceStat::FrameTime_RHIThread:
			return CachedData.RHIThreadTimeSeconds;

		case ESMDisplayablePerformanceStat::FrameTime_GPU:
			return CachedData.GPUTimeSeconds;

		case ESMDisplayablePerformanceStat::Ping:
			return CachedPingMS;

		case ESMDisplayablePerformanceStat::PacketLoss_Incoming:
			return CachedPacketLossIncomingPercent;

		case ESMDisplayablePerformanceStat::PacketLoss_Outgoing:
			return CachedPacketLossOutgoingPercent;

		case ESMDisplayablePerformanceStat::PacketRate_Incoming:
			return CachedPacketRateIncoming;

		case ESMDisplayablePerformanceStat::PacketRate_Outgoing:
			return CachedPacketRateOutgoing;

		case ESMDisplayablePerformanceStat::PacketSize_Incoming:
			return CachedPacketSizeIncoming;

		case ESMDisplayablePerformanceStat::PacketSize_Outgoing:
			return CachedPacketSizeOutgoing;

		default:
			return 0.0f;
	}
}

void USMPerformanceStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Tracker = MakeShared<FSMPerformanceStatCache>(this);
	GEngine->AddPerformanceDataConsumer(Tracker);
}

void USMPerformanceStatSubsystem::Deinitialize()
{
	GEngine->RemovePerformanceDataConsumer(Tracker);
	Tracker.Reset();
}

double USMPerformanceStatSubsystem::GetCachedStat(ESMDisplayablePerformanceStat Stat) const
{
	return Tracker->GetCachedStat(Stat);
}
