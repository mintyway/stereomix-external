// Copyright Studio Surround. All Rights Reserved.


#include "SMPlaylist.h"

#include "CommonTextBlock.h"
#include "Colors/SMColorSet.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "Data/SMTeam.h"
#include "Internationalization/StringTableRegistry.h"
#include "Teams/SMTeamStatics.h"
#include "UI/CharacterSelector/SMCharacterProfile.h"


void USMPlaylist::NativeConstruct()
{
	Super::NativeConstruct();

	WinTeamPlayerImages.Add(WinTeamPlayerType1);
	WinTeamPlayerImages.Add(WinTeamPlayerType2);
	WinTeamPlayerImages.Add(WinTeamPlayerType3);

	WinTeamPlayerNames.Add(WinTeamPlayerName1);
	WinTeamPlayerNames.Add(WinTeamPlayerName2);
	WinTeamPlayerNames.Add(WinTeamPlayerName3);

	LoseTeamPlayerImages.Add(LoseTeamPlayerType1);
	LoseTeamPlayerImages.Add(LoseTeamPlayerType2);
	LoseTeamPlayerImages.Add(LoseTeamPlayerType3);

	LoseTeamPlayerNames.Add(LoseTeamPlayerName1);
	LoseTeamPlayerNames.Add(LoseTeamPlayerName2);
	LoseTeamPlayerNames.Add(LoseTeamPlayerName3);

	EDMRowImages.Add(EDMRowProfile1);
	EDMRowImages.Add(EDMRowProfile2);
	EDMRowImages.Add(EDMRowProfile3);

	EDMRowNames.Add(EDMRowName1);
	EDMRowNames.Add(EDMRowName2);
	EDMRowNames.Add(EDMRowName3);

	FBRowImages.Add(FBRowProfile1);
	FBRowImages.Add(FBRowProfile2);
	FBRowImages.Add(FBRowProfile3);

	FBRowNames.Add(FBRowName1);
	FBRowNames.Add(FBRowName2);
	FBRowNames.Add(FBRowName3);

	OverviewColumnTitles.Add(OverviewColumn1_Title);
	OverviewColumnTitles.Add(OverviewColumn2_Title);
	OverviewColumnTitles.Add(OverviewColumn3_Title);
	OverviewColumnTitles.Add(OverviewColumn4_Title);
	OverviewColumnTitles.Add(OverviewColumn5_Title);
	OverviewColumnTitles.Add(OverviewColumn6_Title);
	OverviewColumnTitles.Add(OverviewColumn7_Title);

	OverviewRows.Add({ OverviewColumn1_1, OverviewColumn2_1, OverviewColumn3_1, OverviewColumn4_1, OverviewColumn5_1, OverviewColumn6_1, OverviewColumn7_1 });
	OverviewRows.Add({ OverviewColumn1_2, OverviewColumn2_2, OverviewColumn3_2, OverviewColumn4_2, OverviewColumn5_2, OverviewColumn6_2, OverviewColumn7_2 });
	OverviewRows.Add({ OverviewColumn1_3, OverviewColumn2_3, OverviewColumn3_3, OverviewColumn4_3, OverviewColumn5_3, OverviewColumn6_3, OverviewColumn7_3 });
	OverviewRows.Add({ OverviewColumn1_4, OverviewColumn2_4, OverviewColumn3_4, OverviewColumn4_4, OverviewColumn5_4, OverviewColumn6_4, OverviewColumn7_4 });
	OverviewRows.Add({ OverviewColumn1_5, OverviewColumn2_5, OverviewColumn3_5, OverviewColumn4_5, OverviewColumn5_5, OverviewColumn6_5, OverviewColumn7_5 });
	OverviewRows.Add({ OverviewColumn1_6, OverviewColumn2_6, OverviewColumn3_6, OverviewColumn4_6, OverviewColumn5_6, OverviewColumn6_6, OverviewColumn7_6 });
}

void USMPlaylist::ResetTeamInfo() const
{
	WinTeamScore->SetText(FText::FromString("0"));
	LoseTeamScore->SetText(FText::FromString("0"));

	for (auto& PlayerImage : WinTeamPlayerImages)
	{
		PlayerImage->SetProfileImage(ESMTeam::None, ESMCharacterType::None);
	}

	for (auto& PlayerName : WinTeamPlayerNames)
	{
		PlayerName->SetText(FText::GetEmpty());
	}

	for (auto& PlayerImage : LoseTeamPlayerImages)
	{
		PlayerImage->SetProfileImage(ESMTeam::None, ESMCharacterType::None);
	}

	for (auto& PlayerName : LoseTeamPlayerNames)
	{
		PlayerName->SetText(FText::GetEmpty());
	}

	for (auto& PlayerImage : EDMRowImages)
	{
		PlayerImage->SetProfileImage(ESMTeam::None, ESMCharacterType::None);
	}

	for (auto& RowName : EDMRowNames)
	{
		RowName->SetText(FText::GetEmpty());
	}

	for (auto& PlayerImage : FBRowImages)
	{
		PlayerImage->SetProfileImage(ESMTeam::None, ESMCharacterType::None);
	}

	for (auto& RowName : FBRowNames)
	{
		RowName->SetText(FText::GetEmpty());
	}

	for (auto& OverviewRow : OverviewRows)
	{
		for (auto& Column : OverviewRow)
		{
			Column->SetText(FText::GetEmpty());
		}
	}
}

void USMPlaylist::SetVictoryTeam(ESMTeam WinTeam)
{
	EDMWinTeamBackground->SetVisibility(WinTeam == ESMTeam::EDM ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	EDMWinAlbum->SetVisibility(WinTeam == ESMTeam::EDM ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	FBWinTeamBackground->SetVisibility(WinTeam == ESMTeam::FutureBass ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	FBWinAlbum->SetVisibility(WinTeam == ESMTeam::FutureBass ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	WinTeamType->SetText(WinTeam == ESMTeam::EDM ? LOCTABLE("Game", "Team_EDM") : LOCTABLE("Game", "Team_FB"));
	LoseTeamType->SetText(WinTeam == ESMTeam::EDM ? LOCTABLE("Game", "Team_FB") : LOCTABLE("Game", "Team_EDM"));

	OverviewEDMTeamResult->SetText(WinTeam == ESMTeam::EDM ? LOCTABLE("UI", "GameResult_Win") : LOCTABLE("UI", "GameResult_Lose"));
	OverviewFBTeamResult->SetText(WinTeam == ESMTeam::FutureBass ? LOCTABLE("UI", "GameResult_Win") : LOCTABLE("UI", "GameResult_Lose"));

	EDMWinCD->SetVisibility(WinTeam == ESMTeam::EDM ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	FBWinCD->SetVisibility(WinTeam == ESMTeam::FutureBass ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	ResetTeamInfo();

	ChangePlaylist(WinTeam);
}

void USMPlaylist::ChangePlaylist(ESMTeam WinTeam)
{
	UCommonTextBlock* EDMTeamScore = LoseTeamScore;
	TArray<USMCharacterProfile*> EDMPlayerImages = LoseTeamPlayerImages;
	TArray<UCommonTextBlock*> EDMPlayerNames = LoseTeamPlayerNames;

	UCommonTextBlock* FBTeamScore = LoseTeamScore;
	TArray<USMCharacterProfile*> FBPlayerImages = LoseTeamPlayerImages;
	TArray<UCommonTextBlock*> FBPlayerNames = LoseTeamPlayerNames;

	switch (WinTeam)
	{
		case ESMTeam::EDM:
			EDMTeamScore = WinTeamScore;
			EDMPlayerImages = WinTeamPlayerImages;
			EDMPlayerNames = WinTeamPlayerNames;
			if (TeamColorSet)
			{
				PlaylistWinTeamResult->SetColorAndOpacity(TeamColorSet->GetTeamLinearColor(ESMTeam::EDM));
				PlaylistLoseTeamResult->SetColorAndOpacity(TeamColorSet->GetTeamLinearColor(ESMTeam::FutureBass));
			}
			break;
		case ESMTeam::FutureBass:
			FBTeamScore = WinTeamScore;
			FBPlayerImages = WinTeamPlayerImages;
			FBPlayerNames = WinTeamPlayerNames;
			if (TeamColorSet)
			{
				PlaylistWinTeamResult->SetColorAndOpacity(TeamColorSet->GetTeamLinearColor(ESMTeam::FutureBass));
				PlaylistLoseTeamResult->SetColorAndOpacity(TeamColorSet->GetTeamLinearColor(ESMTeam::EDM));
			}
			break;
		default:
			break;
	}

	int TotalEDMTeamScore = 0;
	for (int i = 0; i < EDMAnalysisData.Num(); i++)
	{
		TotalEDMTeamScore += EDMAnalysisData[i].TotalScore();
		EDMPlayerImages[i]->SetProfileImage(ESMTeam::EDM, EDMAnalysisData[i].CharacterType);
		EDMPlayerNames[i]->SetText(FText::FromString(EDMAnalysisData[i].PlayerName));
		EDMRowImages[i]->SetProfileImage(ESMTeam::EDM, EDMAnalysisData[i].CharacterType);
		EDMRowNames[i]->SetText(FText::FromString(EDMAnalysisData[i].PlayerName));
	}
	EDMTeamScore->SetText(FText::FromString(FString::FromInt(TotalEDMTeamScore)));

	int TotalFBTeamScore = 0;
	for (int i = 0; i < FBAnalysisData.Num(); i++)
	{
		TotalFBTeamScore += FBAnalysisData[i].TotalScore();
		FBPlayerImages[i]->SetProfileImage(ESMTeam::FutureBass, FBAnalysisData[i].CharacterType);
		FBPlayerNames[i]->SetText(FText::FromString(FBAnalysisData[i].PlayerName));
		FBRowImages[i]->SetProfileImage(ESMTeam::FutureBass, FBAnalysisData[i].CharacterType);
		FBRowNames[i]->SetText(FText::FromString(FBAnalysisData[i].PlayerName));
	}
	FBTeamScore->SetText(FText::FromString(FString::FromInt(TotalFBTeamScore)));
}

void USMPlaylist::ChangeOverview()
{
	SetOverviewColumnTitles({
		FText::GetEmpty(), FText::GetEmpty(), FText::GetEmpty(),
		LOCTABLE("UI", "GameResult_Stat_Score"),
		LOCTABLE("UI", "GameResult_Stat_CapturedTiles"),
		LOCTABLE("UI", "GameResult_Stat_DamageDealt"),
		LOCTABLE("UI", "GameResult_Stat_KillCount")
	});

	for (int i = 0; i < EDMAnalysisData.Num(); i++)
	{
		// 점수를 좌측정렬합니다
		OverviewRows[i][0]->SetJustification(ETextJustify::Center);
		OverviewRows[i][3]->SetJustification(ETextJustify::Left);

		SetOverviewRowNames(OverviewRows[i], {
			TEXT(""), TEXT(""), TEXT(""),
			FString::FromInt(EDMAnalysisData[i].TotalScore()),
			FString::FromInt(EDMAnalysisData[i].TotalCapturedTiles),
			FString::SanitizeFloat(EDMAnalysisData[i].TotalDamageDealt),
			FString::FromInt(EDMAnalysisData[i].TotalKillCount)
		});
	}

	for (int i = 0; i < FBAnalysisData.Num(); i++)
	{
		OverviewRows[i + 3][0]->SetJustification(ETextJustify::Center);
		OverviewRows[i + 3][3]->SetJustification(ETextJustify::Left);

		SetOverviewRowNames(OverviewRows[i + 3], {
			TEXT(""), TEXT(""), TEXT(""),
			FString::FromInt(FBAnalysisData[i].TotalScore()),
			FString::FromInt(FBAnalysisData[i].TotalCapturedTiles),
			FString::SanitizeFloat(FBAnalysisData[i].TotalDamageDealt),
			FString::FromInt(FBAnalysisData[i].TotalKillCount)
		});
	}
}

void USMPlaylist::ChangeAnalytics()
{
	SetOverviewColumnTitles({
		FText::GetEmpty(),
		LOCTABLE("UI", "GameResult_Stat_CapturedTiles"),
		LOCTABLE("UI", "GameResult_Stat_DamageDealt"),
		LOCTABLE("UI", "GameResult_Stat_KillCount"),
		LOCTABLE("UI", "GameResult_Stat_DamageReceived"),
		LOCTABLE("UI", "GameResult_Stat_DeathCount"),
		LOCTABLE("UI", "GameResult_Stat_NoiseBreakUsage")
	});

	for (int i = 0; i < EDMAnalysisData.Num(); i++)
	{
		OverviewRows[i][0]->SetJustification(ETextJustify::Left);
		OverviewRows[i][3]->SetJustification(ETextJustify::Center);

		SetOverviewRowNames(OverviewRows[i], {
			FString::FromInt(EDMAnalysisData[i].TotalScore()),
			FString::FromInt(EDMAnalysisData[i].TotalCapturedTiles),
			FString::SanitizeFloat(EDMAnalysisData[i].TotalDamageDealt),
			FString::FromInt(EDMAnalysisData[i].TotalKillCount),
			FString::SanitizeFloat(EDMAnalysisData[i].TotalDamageReceived),
			FString::FromInt(EDMAnalysisData[i].TotalDeathCount),
			FString::FromInt(EDMAnalysisData[i].TotalNoiseBreakUsage)
		});
	}

	for (int i = 0; i < FBAnalysisData.Num(); i++)
	{
		OverviewRows[i + 3][0]->SetJustification(ETextJustify::Left);
		OverviewRows[i + 3][3]->SetJustification(ETextJustify::Center);
		
		SetOverviewRowNames(OverviewRows[i + 3], {
			FString::FromInt(FBAnalysisData[i].TotalScore()),
			FString::FromInt(FBAnalysisData[i].TotalCapturedTiles),
			FString::SanitizeFloat(FBAnalysisData[i].TotalDamageDealt),
			FString::FromInt(FBAnalysisData[i].TotalKillCount),
			FString::SanitizeFloat(FBAnalysisData[i].TotalDamageReceived),
			FString::FromInt(FBAnalysisData[i].TotalDeathCount),
			FString::FromInt(FBAnalysisData[i].TotalNoiseBreakUsage)
		});
	}
}

void USMPlaylist::SetOverviewColumnTitles(const TArray<FText>& Titles) const
{
	for (int i = 0; i < Titles.Num(); i++)
	{
		OverviewColumnTitles[i]->SetText(Titles[i]);
	}
}

void USMPlaylist::SetOverviewRowNames(TArray<TObjectPtr<UCommonTextBlock>>& Row, const TArray<FString>& Names)
{
	for (int i = 0; i < Names.Num(); i++)
	{
		Row[i]->SetText(FText::FromString(Names[i]));
	}
}

void USMPlaylist::ShowPlaylist(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData)
{
	if (EDMPlayerData.Num() > 0 || FBPlayerData.Num() > 0)
	{
		EDMAnalysisData = EDMPlayerData;
		FBAnalysisData = FBPlayerData;

		AddToViewport(2);

		SetVictoryTeam(WinTeam);
	}
}
