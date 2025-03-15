// Copyright Studio Surround. All Rights Reserved.

#include "GameSettingCollection.h"
#include "SMGameSettingRegistry.h"
#include "CustomSettings/SMSettingValueDiscrete_PerfStat.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Internationalization/StringTableRegistry.h"
#include "Performance/SMPerformanceStatTypes.h"

#define LOCTABLE_ID "Settings"

void USMGameSettingRegistry::AddPerformanceStatPage(UGameSettingCollection* PerfStatsOuterCategory, USMLocalPlayer* InLocalPlayer)
{
	//////////////////////////////////////////////////////////////////
	// Performance Stats
	{
		static_assert(static_cast<int32>(ESMDisplayablePerformanceStat::Count) == 15, "Consider updating this function to deal with new performance stats");

		UGameSettingCollectionPage* StatsPage = NewObject<UGameSettingCollectionPage>();
		StatsPage->SetDevName(TEXT("PerfStatsPage"));
		StatsPage->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat"));
		StatsPage->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_Desc"));
		StatsPage->SetNavigationText(LOCTABLE(LOCTABLE_ID, "Edit"));

		StatsPage->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

		PerfStatsOuterCategory->AddSetting(StatsPage);

		//////////////////////////////////////////////////////////////////
		// Performance
		{
			UGameSettingCollection* StatCategory_Performance = NewObject<UGameSettingCollection>();
			StatCategory_Performance->SetDevName(TEXT("StatCategory_Performance"));
			StatCategory_Performance->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStatCollection"));
			StatsPage->AddSetting(StatCategory_Performance);

			//////////////////////////////////////////////////////////////////
			// Client FPS
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::ClientFPS);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_ClientFPS"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_ClientFPS_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Server FPS
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::ServerFPS);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_ServerFPS"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_ServerFPS_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Frame Time
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::FrameTime);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Idle Time
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::IdleTime);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_IdleTime"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_IdleTime_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// CPU Game Time
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::FrameTime_GameThread);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_GameThread"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_GameThread_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// CPU Render Time
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::FrameTime_RenderThread);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_RenderThread"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_RenderThread_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// CPU RHI Time
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::FrameTime_RHIThread);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_RHIThread"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_RHIThread_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// GPU Render Time
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::FrameTime_GPU);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_GPU"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_FrameTime_GPU_Desc"));
				StatCategory_Performance->AddSetting(Setting);
			}
		}

		//////////////////////////////////////////////////////////////////
		// Network
		{
			UGameSettingCollection* StatCategory_Network = NewObject<UGameSettingCollection>();
			StatCategory_Network->SetDevName(TEXT("StatCategory_Network"));
			StatCategory_Network->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_NetworkCollection"));
			StatsPage->AddSetting(StatCategory_Network);

			//////////////////////////////////////////////////////////////////
			// Ping
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::Ping);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_Ping"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_Ping_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Incoming Packet Loss
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::PacketLoss_Incoming);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketLoss_Incoming"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketLoss_Incoming_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Outgoing Packet Loss
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::PacketLoss_Outgoing);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketLoss_Outgoing"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketLoss_Outgoing_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Incoming Packet Rate
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::PacketRate_Incoming);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketRate_Incoming"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketRate_Incoming_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Outgoing Packet Rate
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::PacketRate_Outgoing);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketRate_Outgoing"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketRate_Outgoing_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Incoming Packet Size
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::PacketSize_Incoming);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketSize_Incoming"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketSize_Incoming_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
			//////////////////////////////////////////////////////////////////
			// Outgoing Packet Size
			{
				USMSettingValueDiscrete_PerfStat* Setting = NewObject<USMSettingValueDiscrete_PerfStat>();
				Setting->SetStat(ESMDisplayablePerformanceStat::PacketSize_Outgoing);
				Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketSize_Outgoing"));
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "PerfStat_PacketSize_Outgoing_Desc"));
				StatCategory_Network->AddSetting(Setting);
			}
		}
	}
}

#undef LOCTABLE_ID
