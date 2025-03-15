// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreboard.h"

#include "CommonTextBlock.h"
#include "SMCharacterIllustration.h"
#include "SMScoreboardAnalytics.h"
#include "SMSimplePlayerScore.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Internationalization/StringTableRegistry.h"
#include "Player/SMPlayerState.h"

void USMScoreboard::ShowScoreboard(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData)
{
	if (const APlayerController* OwningPlayer = GetOwningPlayer())
	{
		if (const ASMPlayerState* PlayerState = OwningPlayer->GetPlayerState<ASMPlayerState>())
		{
			const bool bIsWin = PlayerState->GetTeam() == WinTeam;

			ResultText->SetText(bIsWin ? LOCTABLE("UI", "GameResult_Win") : LOCTABLE("UI", "GameResult_Lose"));
			ResultText->SetColorAndOpacity(bIsWin ? FLinearColor(1.0f, 0.68f, 0.08f, 1.0f) : FLinearColor(0.99f, 0.19f, 0.19f, 1.0f));

			Background->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Team"), PlayerState->GetTeam() == ESMTeam::EDM ? 0.0f : 1.0f);
			Background->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Win"), bIsWin ? 0.0f : 1.0f);

			EDMMVP->SetVisibility(ESMTeam::EDM == WinTeam ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			EDMACE->SetVisibility(ESMTeam::EDM == WinTeam ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
			FBMVP->SetVisibility(ESMTeam::FutureBass == WinTeam ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			FBACE->SetVisibility(ESMTeam::FutureBass == WinTeam ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

			SetCharacterIllustration(PlayerState->GetTeam() == ESMTeam::EDM ? EDMPlayerData : FBPlayerData);
		}
	}

	int32 EDMTotalScore = 0;
	for (const FPlayerScoreData& PlayerData : EDMPlayerData)
	{
		EDMTotalScore += PlayerData.TotalScore();
	}
	EDMScore->SetText(FText::AsNumber(EDMTotalScore));

	int32 FBTotalScore = 0;
	for (const FPlayerScoreData& PlayerData : FBPlayerData)
	{
		FBTotalScore += PlayerData.TotalScore();
	}
	FBScore->SetText(FText::AsNumber(FBTotalScore));

	float EDMPercent = 0.0f;
	float FBPercent = 0.0f;
	if (const int32 MaxScore = FMath::Max(EDMTotalScore, FBTotalScore); MaxScore > 0)
	{
		EDMPercent = EDMTotalScore > 0 ? 1.0f * EDMTotalScore / MaxScore : 0.0f;
		FBPercent = FBTotalScore > 0 ? 1.0f * FBTotalScore / MaxScore : 0.0f;
	}

	EDMScoreProgress->SetPercent(EDMPercent);
	FBScoreProgress->SetPercent(FBPercent);

	auto SetPlayerScoreData = [&](const TArray<FPlayerScoreData>& TeamData, const TArray<USMSimplePlayerScore*>& PlayerScores) {
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

	const TArray<USMSimplePlayerScore*> EDMTeamScores = { EDMPlayerScore1, EDMPlayerScore2, EDMPlayerScore3 };
	const TArray<USMSimplePlayerScore*> FBTeamScores = { FBPlayerScore1, FBPlayerScore2, FBPlayerScore3 };

	SetPlayerScoreData(EDMPlayerData, EDMTeamScores);
	SetPlayerScoreData(FBPlayerData, FBTeamScores);

	if (EDMPlayerData.Num() > 0 || FBPlayerData.Num() > 0)
	{
		EDMAnalyticsData = EDMPlayerData;
		FBAnalyticsData = FBPlayerData;

		const TArray<FPlayerScoreData> WinTeamData = WinTeam == ESMTeam::EDM ? EDMPlayerData : FBPlayerData;
		const TArray<FPlayerScoreData> LoseTeamData = WinTeam == ESMTeam::EDM ? FBPlayerData : EDMPlayerData;

		ScoreboardAnalytics->SetAnalyticsData(WinTeam, WinTeamData, LoseTeamData);

		AddToViewport(2);
	}
}

void USMScoreboard::SetCharacterIllustration(const TArray<FPlayerScoreData>& PlayerData) const
{
	TArray<USMCharacterIllustration*> CharacterIllustrations = { CharacterIllustration1, CharacterIllustration2, CharacterIllustration3 };
	for (int32 i = 0; i < CharacterIllustrations.Num(); ++i)
	{
		if (i < PlayerData.Num())
		{
			CharacterIllustrations[i]->SetCharacterIllustration(PlayerData[i].PlayerTeam, PlayerData[i].CharacterType);
		}
		else
		{
			CharacterIllustrations[i]->SetCharacterIllustration(ESMTeam::None, ESMCharacterType::None);
		}
	}
}
