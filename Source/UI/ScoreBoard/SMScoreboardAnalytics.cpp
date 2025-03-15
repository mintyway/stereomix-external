// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreboardAnalytics.h"

#include "CommonTextBlock.h"
#include "SMPlayerScore.h"
#include "Colors/SMColorSet.h"
#include "Teams/SMTeamDefinition.h"
#include "Teams/SMTeamStatics.h"

void USMScoreboardAnalytics::SetAnalyticsData(const ESMTeam WinTeam, const TArray<FPlayerScoreData>& WinTeamData, const TArray<FPlayerScoreData>& LoseTeamData)
{
	auto SetPlayerScoreData = [&](const TArray<FPlayerScoreData>& TeamData, const TArray<USMPlayerScore*>& PlayerScores) {
		for (int32 i = 0; i < PlayerScores.Num(); ++i)
		{
			if (i < TeamData.Num())
			{
				PlayerScores[i]->SetPlayerScore(TeamData[i], false);
			}
			else
			{
				PlayerScores[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	};

	FString EDMName = USMTeamStatics::GetTeamDefinition(ESMTeam::EDM)->DisplayName.ToString().ToUpper();
	FString FBName = USMTeamStatics::GetTeamDefinition(ESMTeam::FutureBass)->DisplayName.ToString().ToUpper();
	TeamName1->SetText(WinTeam == ESMTeam::EDM ? FText::FromString(EDMName) : FText::FromString(FBName));
	if (TeamColorSet)
	{
		TeamName1->SetColorAndOpacity(WinTeam == ESMTeam::EDM ? TeamColorSet->GetTeamLinearColor(ESMTeam::EDM) : TeamColorSet->GetTeamLinearColor(ESMTeam::FutureBass));
	}
	TeamName2->SetText(WinTeam == ESMTeam::EDM ? FText::FromString(FBName) : FText::FromString(EDMName));
	if (TeamColorSet)
	{
		TeamName2->SetColorAndOpacity(WinTeam == ESMTeam::EDM ? TeamColorSet->GetTeamLinearColor(ESMTeam::FutureBass) : TeamColorSet->GetTeamLinearColor(ESMTeam::EDM));
	}

	const TArray<USMPlayerScore*> WinTeamScores = { PlayerScore1, PlayerScore2, PlayerScore3 };
	const TArray<USMPlayerScore*> LoseTeamScores = { PlayerScore4, PlayerScore5, PlayerScore6 };

	SetPlayerScoreData(WinTeamData, WinTeamScores);
	SetPlayerScoreData(LoseTeamData, LoseTeamScores);
}

void USMScoreboardAnalytics::Show()
{
	PlayAnimation(ShowAnalytics);
}

void USMScoreboardAnalytics::Hide()
{
	PlayAnimation(HideAnalytics);
}
