// Copyright Studio Surround. All Rights Reserved.


#include "SMTeamStatics.h"

#include "SMTeamDefinition.h"
#include "SMTeamSettings.h"

const USMTeamDefinition* USMTeamStatics::GetTeamDefinition(ESMTeam Team)
{
	if (const USMTeamSettings* TeamSettings = GetDefault<USMTeamSettings>())
	{
		if (const TSoftObjectPtr<USMTeamDefinition>* TeamDefinitionPtr = TeamSettings->TeamDefinitions.Find(Team))
		{
			return TeamDefinitionPtr->LoadSynchronous();
		}
	}
	return nullptr;
}
