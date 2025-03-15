// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingValueDiscrete_PerfStat.h"

#include "CommonUIVisibilitySubsystem.h"
#include "Internationalization/StringTableRegistry.h"
#include "Performance/SMPerformanceSettings.h"
#include "Performance/SMPerformanceStatTypes.h"
#include "Settings/SMSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingValueDiscrete_PerfStat)

#define LOCTABLE_ID "Settings"

class FGameSettingEditCondition_PerfStatAllowed : public FGameSettingEditCondition
{
public:
	FGameSettingEditCondition_PerfStatAllowed(ESMDisplayablePerformanceStat Stat)
		: AssociatedStat(Stat)
	{
	}

	static TSharedRef<FGameSettingEditCondition_PerfStatAllowed> Get(ESMDisplayablePerformanceStat Stat)
	{
		return MakeShared<FGameSettingEditCondition_PerfStatAllowed>(Stat);
	}

	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		const FGameplayTagContainer& VisibilityTags = UCommonUIVisibilitySubsystem::GetChecked(InLocalPlayer)->GetVisibilityTags();

		bool bCanShowStat = false;
		for (const FSMPerformanceStatGroup& Group : GetDefault<USMPerformanceSettings>()->UserFacingPerformanceStats)
		{
			if (Group.AllowedStats.Contains(AssociatedStat))
			{
				// is show group
				if (Group.VisibilityQuery.IsEmpty() || VisibilityTags.HasAny(VisibilityTags))
				{
					bCanShowStat = true;
					break;
				}
			}
		}
		if (!bCanShowStat)
		{
			InOutEditState.Hide(TEXT("Stat is not listed in USMPerformanceSettings or is suppressed by current platform traits"));
		}
	}

private:
	ESMDisplayablePerformanceStat AssociatedStat;
};

USMSettingValueDiscrete_PerfStat::USMSettingValueDiscrete_PerfStat()
	: StatToDisplay(), InitialMode()
{
}

void USMSettingValueDiscrete_PerfStat::OnInitialized()
{
	Super::OnInitialized();
	AddMode(LOCTABLE(LOCTABLE_ID, "PerfStat_Hide"), ESMStatDisplayMode::Hidden);
	AddMode(LOCTABLE(LOCTABLE_ID, "PerfStat_TextOnly"), ESMStatDisplayMode::TextOnly);

	// 그래프는 아직 미구현
	// AddMode(LOCTABLE(LOCTABLE_ID, "PerfStat_GraphOnly"), ESMStatDisplayMode::GraphOnly);
	// AddMode(LOCTABLE(LOCTABLE_ID, "PerfStat_TextAndGraph"), ESMStatDisplayMode::TextAndGraph);
}

void USMSettingValueDiscrete_PerfStat::StoreInitial()
{
	const USMSettingsLocal* Settings = USMSettingsLocal::Get();
	InitialMode = Settings->GetPerfStatDisplayState(StatToDisplay);
}

void USMSettingValueDiscrete_PerfStat::ResetToDefault()
{
	USMSettingsLocal* Settings = USMSettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, ESMStatDisplayMode::Hidden);
	NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
}

void USMSettingValueDiscrete_PerfStat::RestoreToInitial()
{
	USMSettingsLocal* Settings = USMSettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, InitialMode);
	NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
}

int32 USMSettingValueDiscrete_PerfStat::GetDiscreteOptionIndex() const
{
	const USMSettingsLocal* Settings = USMSettingsLocal::Get();
	return DisplayModes.Find(Settings->GetPerfStatDisplayState(StatToDisplay));
}

void USMSettingValueDiscrete_PerfStat::SetDiscreteOptionByIndex(int32 Index)
{
	if (DisplayModes.IsValidIndex(Index))
	{
		const ESMStatDisplayMode DisplayMode = DisplayModes[Index];
		USMSettingsLocal* Settings = USMSettingsLocal::Get();
		Settings->SetPerfStatDisplayState(StatToDisplay, DisplayMode);
	}
	NotifySettingChanged(EGameSettingChangeReason::Change);
}

TArray<FText> USMSettingValueDiscrete_PerfStat::GetDiscreteOptions() const
{
	return Options;
}

void USMSettingValueDiscrete_PerfStat::SetStat(ESMDisplayablePerformanceStat InStat)
{
	StatToDisplay = InStat;
	SetDevName(FName(*FString::Printf(TEXT("PerfStat_%d"), static_cast<int32>(InStat))));
	AddEditCondition(FGameSettingEditCondition_PerfStatAllowed::Get(StatToDisplay));
}

void USMSettingValueDiscrete_PerfStat::AddMode(FText&& Label, ESMStatDisplayMode Mode)
{
	Options.Emplace(MoveTemp(Label));
	DisplayModes.Add(Mode);
}

#undef LOCTABLE_ID
