// Copyright Studio Surround. All Rights Reserved.


#include "SMGuideKeyContainer.h"

#include "EnhancedInputSubsystems.h"
#include "StereoMixLog.h"
#include "Components/DynamicEntryBox.h"
#include "UI/SMUITypes.h"
#include "UserSettings/EnhancedInputUserSettings.h"


void USMGuideKeyContainer::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeContainer();
}

void USMGuideKeyContainer::InitializeContainer()
{
	if (!ensure(GuideKeyBox))
	{
		return;
	}

	GuideKeyBox->Reset();

	TSubclassOf<USMGuideKeyWidget> WidgetClass = GuideKeyWidgetClass.LoadSynchronous();
	if (!WidgetClass)
	{
		UE_LOG(LogStereoMixUI, Warning, TEXT("Invalid Guide Key Widget Class in %s"), *GetNameSafe(this));
		return;
	}

	ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	const UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	const UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();

	TMap<FGameplayTag, TObjectPtr<UEnhancedPlayerMappableKeyProfile>> KeyProfiles = UserSettings->GetAllSavedKeyProfiles();
	for (const auto& ProfilePair : KeyProfiles)
	{
		const FGameplayTag& ProfileName = ProfilePair.Key;
		const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

		for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows())
		{
			if (RowPair.Value.HasAnyMappings())
			{
				const UInputAction* Action = RowPair.Value.Mappings.begin()->GetAssociatedInputAction();
				const USMInputMetadata* Metadata = Cast<USMInputMetadata>(CommonUI::GetEnhancedInputActionMetadata(Action));
				if (!Metadata)
				{
					UE_LOG(LogStereoMixUI, Warning, TEXT("No Metadata found for action %s"), *GetNameSafe(Action));
					continue;
				}

				if (!Metadata->bShowGuide)
				{
					continue;
				}

				USMGuideKeyWidget* GuideKeyWidget = GuideKeyBox->CreateEntry<USMGuideKeyWidget>(WidgetClass);
				if (!GuideKeyWidget)
				{
					UE_LOG(LogStereoMixUI, Warning, TEXT("Failed to create Guide Key Widget in %s"), *GetNameSafe(this));
					continue;
				}
				GuideKeyWidget->InitializeAction(Action);
			}
		}
	}
}
