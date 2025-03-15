// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Colors/SMColorSet.h"
#include "SMTeamSettings.generated.h"

enum class ESMTeam : uint8;
class USMTeamDefinition;


UCLASS(Config = Game, DefaultConfig, DisplayName = "StereoMix Team Settings")
class STEREOMIX_API USMTeamSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Team")
	TMap<ESMTeam, TSoftObjectPtr<USMTeamDefinition>> TeamDefinitions;
};
