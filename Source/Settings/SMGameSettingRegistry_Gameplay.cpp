// Copyright Studio Surround. All Rights Reserved.

#include "GameSettingCollection.h"
#include "SMGameSettingRegistry.h"
#include "CustomSettings/SMSettingValueDiscrete_Language.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Internationalization/StringTableRegistry.h"

#define LOCTABLE_ID "Settings"

UGameSettingCollection* USMGameSettingRegistry::InitializeGameplaySettings(USMLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("GameplayCollection"));
	Screen->SetDisplayName(LOCTABLE(LOCTABLE_ID, "GameplayCollection"));
	Screen->Initialize(InLocalPlayer);

	//////////////////////////////////////////////////////////////////
	// Language
	{
		UGameSettingCollection* LanguageSubsection = NewObject<UGameSettingCollection>();
		LanguageSubsection->SetDevName(TEXT("LanguageCollection"));
		LanguageSubsection->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gameplay_LanguageCollection"));
		Screen->AddSetting(LanguageSubsection);

		//////////////////////////////////////////////////////////////////
		// Language
		{
			USMSettingValueDiscrete_Language* Setting = NewObject<USMSettingValueDiscrete_Language>();
			Setting->SetDevName(TEXT("Language"));
			Setting->SetDisplayName(LOCTABLE(LOCTABLE_ID, "Gameplay_LanguageSetting"));
			Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gameplay_LanguageSetting_Desc"));

#if WITH_EDITOR
			if (GIsEditor)
			{
				Setting->SetDescriptionRichText(LOCTABLE(LOCTABLE_ID, "Gameplay_LanguageSetting_WithEditor_Desc"));
			}
#endif
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			LanguageSubsection->AddSetting(Setting);
		}
	}
	
	return Screen;
}

#undef LOCTABLE_ID
