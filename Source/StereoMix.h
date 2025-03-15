// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FStereoMixModule final : public FDefaultGameModuleImpl
{
	using ThisClass = FStereoMixModule;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};