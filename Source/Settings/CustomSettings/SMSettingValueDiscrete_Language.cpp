// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingValueDiscrete_Language.h"

#include "Internationalization/Culture.h"
#include "Internationalization/StringTableRegistry.h"
#include "Messaging/CommonGameDialog.h"
#include "Messaging/CommonMessagingSubsystem.h"
#include "Settings/SMSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingValueDiscrete_Language)

#define LOCTABLE_ID "Settings"

static constexpr int32 SettingSystemDefaultLanguageIndex = 0;

USMSettingValueDiscrete_Language::USMSettingValueDiscrete_Language()
{
}

void USMSettingValueDiscrete_Language::OnInitialized()
{
	Super::OnInitialized();
	const TArray<FString> AllCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Game);
	for (const FString& CultureName : AllCultureNames)
	{
		if (FInternationalization::Get().IsCultureAllowed(CultureName))
		{
			AvailableCultureNames.Add(CultureName);
		}
	}

	AvailableCultureNames.Insert(TEXT(""), SettingSystemDefaultLanguageIndex);
}

void USMSettingValueDiscrete_Language::StoreInitial()
{
}

void USMSettingValueDiscrete_Language::OnApply()
{
	if (USMSettingsShared* Settings = CastChecked<USMLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		Settings->ApplyCultureSettings();
	}

	if (UCommonMessagingSubsystem* Messaging = LocalPlayer->GetSubsystem<UCommonMessagingSubsystem>())
	{
		Messaging->ShowConfirmation(
			UCommonGameDialogDescriptor::CreateConfirmationOk(
				LOCTABLE(LOCTABLE_ID, "Gameplay_Language_Applied_Title"),
				LOCTABLE(LOCTABLE_ID, "Gameplay_Language_Applied_Message")
				));
	}
}

void USMSettingValueDiscrete_Language::ResetToDefault()
{
	SetDiscreteOptionByIndex(SettingSystemDefaultLanguageIndex);
}

void USMSettingValueDiscrete_Language::RestoreToInitial()
{
	if (USMSettingsShared* Settings = CastChecked<USMLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		Settings->ClearPendingCulture();
		NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
	}
}

int32 USMSettingValueDiscrete_Language::GetDiscreteOptionIndex() const
{
	if (const USMSettingsShared* Settings = CastChecked<USMLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		if (Settings->ShouldResetToDefaultCulture())
		{
			return SettingSystemDefaultLanguageIndex;
		}

		// We prefer the pending culture to the current culture as the options UI updates the pending culture before it 
		// gets applied, and we need the UI to reflect that choice
		FString PendingCulture = Settings->GetPendingCulture();
		if (PendingCulture.IsEmpty())
		{
			if (Settings->IsUsingDefaultCulture())
			{
				return SettingSystemDefaultLanguageIndex;
			}

			PendingCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
		}

		// Try to find an exact match 
		{
			const int32 ExactMatchIndex = AvailableCultureNames.IndexOfByKey(PendingCulture);
			if (ExactMatchIndex != INDEX_NONE)
			{
				return ExactMatchIndex;
			}
		}

		// Try to find a prioritized match (eg, allowing "en-US" to show as "en" in the UI)
		const TArray<FString> PrioritizedPendingCultures = FInternationalization::Get().GetPrioritizedCultureNames(PendingCulture);
		for (int32 i = 0; i < AvailableCultureNames.Num(); ++i)
		{
			if (PrioritizedPendingCultures.Contains(AvailableCultureNames[i]))
			{
				return i;
			}
		}
	}

	return 0;
}

void USMSettingValueDiscrete_Language::SetDiscreteOptionByIndex(int32 Index)
{
	if (USMSettingsShared* Settings = CastChecked<USMLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		if (Index == SettingSystemDefaultLanguageIndex)
		{
			Settings->ResetToDefaultCulture();
		}
		else if (AvailableCultureNames.IsValidIndex(Index))
		{
			Settings->SetPendingCulture(AvailableCultureNames[Index]);
		}
		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

TArray<FText> USMSettingValueDiscrete_Language::GetDiscreteOptions() const
{
	TArray<FText> Options;

	for (const FString& CultureName : AvailableCultureNames)
	{
		if (CultureName == TEXT(""))
		{
			const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
			if (ensure(SystemDefaultCulture))
			{
				const FString& DefaultCultureDisplayName = SystemDefaultCulture->GetDisplayName();
				FText LocalizedSystemDefault = FText::Format(LOCTABLE(LOCTABLE_ID, "Gameplay_Language_SystemDefault"), FText::FromString(DefaultCultureDisplayName));
				Options.Add(MoveTemp(LocalizedSystemDefault));
			}
		}
		else
		{
			const FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
			if (ensureMsgf(Culture, TEXT("Unable to find Culture '%s'!"), *CultureName))
			{
				const FString CultureDisplayName = Culture->GetDisplayName();
				const FString CultureNativeName = Culture->GetNativeName();

				// Only show both names if they're different (to avoid repetition)
				FString Entry = !CultureNativeName.Equals(CultureDisplayName, ESearchCase::CaseSensitive)
					                ? FString::Printf(TEXT("%s (%s)"), *CultureNativeName, *CultureDisplayName)
					                : CultureNativeName;

				Options.Add(FText::FromString(Entry));
			}
		}
	}

	return Options;
}

#undef LOCTABLE_ID
