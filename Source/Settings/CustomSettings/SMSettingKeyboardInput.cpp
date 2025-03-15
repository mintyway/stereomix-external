// Copyright Studio Surround. All Rights Reserved.


#include "SMSettingKeyboardInput.h"

#include "EnhancedInputSubsystems.h"
#include "Internationalization/StringTableRegistry.h"
#include "Player/SMLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMSettingKeyboardInput)

#define LOCTABLE_ID "Settings"

namespace StereoMix::ErrorMessages
{
	static const FText UnknownMappingName = LOCTABLE(LOCTABLE_ID, "Binding_UnknownMapping");
}

USMSettingKeyboardInput::USMSettingKeyboardInput()
{
	bReportAnalytics = false;
}

void USMSettingKeyboardInput::OnInitialized()
{
	DynamicDetails = FGetGameSettingsDetails::CreateLambda([this](ULocalPlayer&) {
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			if (Row->HasAnyMappings())
			{
				return FText::Format(LOCTABLE(LOCTABLE_ID, "Binding_InputAction"), Row->Mappings.begin()->GetDisplayName());
			}
		}
		return FText::GetEmpty();
	});
	Super::OnInitialized();
}

void USMSettingKeyboardInput::StoreInitial()
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptions))
				{
					ActionMappingName = Mapping.GetMappingName();
					InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());
				}
			}
		}
	}
}

void USMSettingKeyboardInput::ResetToDefault()
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;
		FGameplayTagContainer FailureReason;
		Settings->ResetAllPlayerKeysInRow(Args, FailureReason);
		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

void USMSettingKeyboardInput::RestoreToInitial()
{
	for (auto Pair : InitialKeyMappings)
	{
		ChangeBinding(static_cast<int32>(Pair.Key), Pair.Value);
	}
}

void USMSettingKeyboardInput::InitializeInputData(const UEnhancedPlayerMappableKeyProfile* KeyProfile, const FKeyMappingRow& MappingData, const FPlayerMappableKeyQueryOptions& InQueryOptions)
{
	check(KeyProfile);

	ProfileIdentifier = KeyProfile->GetProfileIdentifer();
	QueryOptions = InQueryOptions;

	for (const FPlayerKeyMapping& Mapping : MappingData.Mappings)
	{
		// Only add mappings that pass the query filters that have been provided upon creation
		if (!KeyProfile->DoesMappingPassQueryOptions(Mapping, InQueryOptions))
		{
			continue;
		}

		ActionMappingName = Mapping.GetMappingName();
		InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());
		const FText& MappingDisplayName = Mapping.GetDisplayName();

		if (!MappingDisplayName.IsEmpty())
		{
			SetDisplayName(MappingDisplayName);
		}
	}

	const FString NameString = TEXT("KBM_Input_") + ActionMappingName.ToString();
	SetDevName(*NameString);
}

FText USMSettingKeyboardInput::GetKeyTextFromSlot(const EPlayerMappableKeySlot InSlot) const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;
		QueryOptionsForSlot.SlotToMatch = InSlot;

		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					return Mapping.GetCurrentKey().GetDisplayName();
				}
			}
		}
	}
	return EKeys::Invalid.GetDisplayName();
}

bool USMSettingKeyboardInput::ChangeBinding(int32 InKeyBindSlot, FKey NewKey)
{
	if (!NewKey.IsGamepadKey())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;
		Args.Slot = static_cast<EPlayerMappableKeySlot>(static_cast<uint8>(InKeyBindSlot));
		Args.NewKey = NewKey;
		// If you want to, you can additionally specify this mapping to only be applied to a certain hardware device or key profile
		//Args.ProfileId =
		//Args.HardwareDeviceId =
		if (UEnhancedInputUserSettings* Settings = GetUserSettings())
		{
			FGameplayTagContainer FailureReason;
			Settings->MapPlayerKey(Args, FailureReason);
			NotifySettingChanged(EGameSettingChangeReason::Change);
		}
		return true;
	}
	return false;
}

void USMSettingKeyboardInput::GetAllMappedActionsFromKey(int32 InKeyBindSlot, FKey Key, TArray<FName>& OutActionNames) const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		Profile->GetMappingNamesForKey(Key, OutActionNames);
	}
}

bool USMSettingKeyboardInput::IsMappingCustomized() const
{
	bool bResult = false;
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					bResult |= Mapping.IsCustomized();
				}
			}
		}
	}
	return bResult;
}

FText USMSettingKeyboardInput::GetSettingDisplayName() const
{
	if (const FKeyMappingRow* Row = FindKeyMappingRow())
	{
		if (Row->HasAnyMappings())
		{
			return Row->Mappings.begin()->GetDisplayName();
		}
	}
	return StereoMix::ErrorMessages::UnknownMappingName;
}

FText USMSettingKeyboardInput::GetSettingDisplayCategory() const
{
	if (const FKeyMappingRow* Row = FindKeyMappingRow())
	{
		if (Row->HasAnyMappings())
		{
			return Row->Mappings.begin()->GetDisplayCategory();
		}
	}
	return StereoMix::ErrorMessages::UnknownMappingName;
}

const FKeyMappingRow* USMSettingKeyboardInput::FindKeyMappingRow() const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		return Profile->FindKeyMappingRow(ActionMappingName);
	}
	ensure(false);
	return nullptr;
}

UEnhancedPlayerMappableKeyProfile* USMSettingKeyboardInput::FindMappableKeyProfile() const
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings())
	{
		return Settings->GetKeyProfileWithIdentifier(ProfileIdentifier);
	}
	ensure(false);
	return nullptr;
}

UEnhancedInputUserSettings* USMSettingKeyboardInput::GetUserSettings() const
{
	if (USMLocalPlayer* StereoMixLocalPlayer = Cast<USMLocalPlayer>(LocalPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = StereoMixLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			return Subsystem->GetUserSettings();
		}
	}
	return nullptr;
}

#undef LOCTABLE_ID
