// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingValueDiscrete_OverallQuality.h"

#include "Internationalization/StringTableRegistry.h"
#include "Settings/SMSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingValueDiscrete_OverallQuality)

#define LOCTABLE_ID "Settings"

USMSettingValueDiscrete_OverallQuality::USMSettingValueDiscrete_OverallQuality()
{
}

void USMSettingValueDiscrete_OverallQuality::OnInitialized()
{
	Super::OnInitialized();
	USMSettingsLocal* UserSettings = USMSettingsLocal::Get();
	const int32 MaxQualityLevel = UserSettings->GetMaxSupportedOverallQualityLevel();

	auto AddOptionIfPossible = [&](int Index, FText&& Value) { if ((MaxQualityLevel < 0) || (Index <= MaxQualityLevel)) { Options.Add(Value); } };

	AddOptionIfPossible(0, LOCTABLE(LOCTABLE_ID, "Options_Low"));
	AddOptionIfPossible(1, LOCTABLE(LOCTABLE_ID, "Options_Medium"));
	AddOptionIfPossible(2, LOCTABLE(LOCTABLE_ID, "Options_High"));
	AddOptionIfPossible(3, LOCTABLE(LOCTABLE_ID, "Options_Epic"));

	OptionsWithCustom = Options;
	OptionsWithCustom.Add(LOCTABLE(LOCTABLE_ID, "Options_Custom"));
}

void USMSettingValueDiscrete_OverallQuality::StoreInitial()
{
}

void USMSettingValueDiscrete_OverallQuality::ResetToDefault()
{
}

void USMSettingValueDiscrete_OverallQuality::RestoreToInitial()
{
}

int32 USMSettingValueDiscrete_OverallQuality::GetDiscreteOptionIndex() const
{
	const int32 OverallQualityLevel = GetOverallQualityLevel();
	if (OverallQualityLevel == INDEX_NONE)
	{
		return GetCustomOptionIndex();
	}
	return OverallQualityLevel;
}

void USMSettingValueDiscrete_OverallQuality::SetDiscreteOptionByIndex(int32 Index)
{
	UGameUserSettings* UserSettings = CastChecked<UGameUserSettings>(GEngine->GetGameUserSettings());
	if (Index == GetCustomOptionIndex())
	{
		// Leave everything as is we're in a custom setup.
	}
	else
	{
		// Low / Medium / High / Epic
		UserSettings->SetOverallScalabilityLevel(Index);
	}
	NotifySettingChanged(EGameSettingChangeReason::Change);
}

TArray<FText> USMSettingValueDiscrete_OverallQuality::GetDiscreteOptions() const
{
	const int32 OverallQualityLevel = GetOverallQualityLevel();
	if (OverallQualityLevel == INDEX_NONE)
	{
		return OptionsWithCustom;
	}
	return Options;
}

int32 USMSettingValueDiscrete_OverallQuality::GetCustomOptionIndex() const
{
	return OptionsWithCustom.Num() - 1;
}

int32 USMSettingValueDiscrete_OverallQuality::GetOverallQualityLevel() const
{
	const UGameUserSettings* UserSettings = CastChecked<const UGameUserSettings>(GEngine->GetGameUserSettings());
	return UserSettings->GetOverallScalabilityLevel();
}

#undef LOCTABLE_ID
