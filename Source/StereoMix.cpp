// Copyright Epic Games, Inc. All Rights Reserved.

#include "StereoMix.h"

#include "Internationalization/StringTableRegistry.h"
#include "Modules/ModuleManager.h"


void FStereoMixModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	LOCTABLE_FROMFILE_GAME("Game", "Game", "Localization/GameStringTable.csv");
	LOCTABLE_FROMFILE_GAME("UI", "UI", "Localization/UIStringTable.csv");
	LOCTABLE_FROMFILE_GAME("Input", "Input", "Localization/InputStringTable.csv");
	LOCTABLE_FROMFILE_GAME("Settings", "Settings", "Localization/SettingsStringTable.csv");
}

void FStereoMixModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FStereoMixModule, StereoMix, "StereoMix");
