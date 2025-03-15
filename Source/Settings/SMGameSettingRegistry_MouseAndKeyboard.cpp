// Copyright Studio Surround. All Rights Reserved.

#include "CommonInputBaseTypes.h"
#include "EnhancedInputSubsystems.h"
#include "GameSettingCollection.h"
#include "SMGameSettingRegistry.h"
#include "SMSettingsShared.h"
#include "CustomSettings/SMSettingKeyboardInput.h"
#include "EditCondition/WhenCondition.h"
#include "Internationalization/StringTableRegistry.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#define LOCTABLE_ID "Settings"

UGameSettingCollection* USMGameSettingRegistry::InitializeMouseAndKeyboardSettings(USMLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("MouseAndKeyboardCollection"));
	Screen->SetDisplayName(LOCTABLE(LOCTABLE_ID, "MouseAndKeyboardCollection"));
	Screen->Initialize(InLocalPlayer);

	const TSharedRef<FWhenCondition> WhenPlatformSupportsMouseAndKeyboard = MakeShared<FWhenCondition>(
		[](const ULocalPlayer*, FGameSettingEditableState& InOutEditState) {
			const UCommonInputPlatformSettings* PlatformInput = UPlatformSettingsManager::Get().GetSettingsForPlatform<UCommonInputPlatformSettings>();
			if (!PlatformInput->SupportsInputType(ECommonInputType::MouseAndKeyboard))
			{
				InOutEditState.Kill(TEXT("Platform does not support mouse and keyboard"));
			}
		});

	//////////////////////////////////////////////////////////////////
	// Bindings for Mouse & Keyboard - Automatically Generated
	{
		UGameSettingCollection* KeyBinding = NewObject<UGameSettingCollection>();
		KeyBinding->SetDevName(TEXT("KeyBindingCollection"));
		KeyBinding->SetDisplayName(LOCTABLE(LOCTABLE_ID, "KeyBindingCollection"));
		Screen->AddSetting(KeyBinding);

		const UEnhancedInputLocalPlayerSubsystem* EISubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		const UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();

		// If you want to just get one profile pair, then you can do UserSettings->GetCurrentProfile

		// A map of key bindings mapped to their display category
		TMap<FString, UGameSettingCollection*> CategoryToSettingCollection;

		// Returns an existing setting collection for the display category if there is one.
		// If there isn't one, then it will create a new one and initialize it
		auto GetOrCreateSettingCollection = [&CategoryToSettingCollection, &Screen](FText DisplayCategory) -> UGameSettingCollection* {
			static const FString DefaultDevName = TEXT("Default_KBM");

			if (DisplayCategory.IsEmpty())
			{
				DisplayCategory = FText::FromString(TEXT("Default Experience"));
			}

			FString DisplayCatString = DisplayCategory.ToString();

			if (UGameSettingCollection** ExistingCategory = CategoryToSettingCollection.Find(DisplayCatString))
			{
				return *ExistingCategory;
			}

			UGameSettingCollection* ConfigSettingCollection = NewObject<UGameSettingCollection>();
			ConfigSettingCollection->SetDevName(FName(DisplayCatString));
			ConfigSettingCollection->SetDisplayName(DisplayCategory);
			Screen->AddSetting(ConfigSettingCollection);
			CategoryToSettingCollection.Add(DisplayCatString, ConfigSettingCollection);

			return ConfigSettingCollection;
		};

		static TSet<FName> CreatedMappingNames;
		CreatedMappingNames.Reset();

		TMap<FGameplayTag, TObjectPtr<UEnhancedPlayerMappableKeyProfile>> KeyProfiles = UserSettings->GetAllSavedKeyProfiles();
		for (const TPair<FGameplayTag, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : KeyProfiles)
		{
			const FGameplayTag& ProfileName = ProfilePair.Key;
			const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

			// 주의!: EnhanceInputAction에 DisplayName이 없으면 ensure 컨디션에 걸립니다. DisplayName을 꼭 설정해주세요.

			for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows())
			{
				// Create a setting row for anything with valid mappings and that we haven't created yet
				if (RowPair.Value.HasAnyMappings() /* && !CreatedMappingNames.Contains(RowPair.Key)*/)
				{
					// We only want keyboard keys on this settings screen, so we will filter down by mappings
					// that are set to keyboard keys
					FPlayerMappableKeyQueryOptions Options = {};
					Options.KeyToMatch = EKeys::W;
					Options.bMatchBasicKeyTypes = true;

					const FText& DesiredDisplayCategory = RowPair.Value.Mappings.begin()->GetDisplayCategory();

					if (UGameSettingCollection* Collection = GetOrCreateSettingCollection(DesiredDisplayCategory))
					{
						// Create the settings widget and initialize it, adding it to this config's section
						USMSettingKeyboardInput* InputBinding = NewObject<USMSettingKeyboardInput>();

						InputBinding->InitializeInputData(Profile, RowPair.Value, Options);
						InputBinding->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

						Collection->AddSetting(InputBinding);
						CreatedMappingNames.Add(RowPair.Key);
					}
					else
					{
						ensure(false);
					}
				}
			}
		}
	}

	return Screen;
}

#undef LOCTABLE_ID
