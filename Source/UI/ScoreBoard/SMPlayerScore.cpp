// Copyright Studio Surround. All Rights Reserved.


#include "SMPlayerScore.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Internationalization/StringTableRegistry.h"
#include "UI/CharacterSelector/SMCharacterProfile.h"

void USMPlayerScore::SetPlayerScore(const FPlayerScoreData& PlayerScoreData, const bool bIsOwningPlayer) const
{
	switch (PlayerScoreData.PlayerTeam)
	{
		case ESMTeam::None:
			break;
		case ESMTeam::EDM:
			ScoreBox->SetColorAndOpacity(FLinearColor(0.45f, 0.86f, 0.74f, 1.0f));
			break;
		case ESMTeam::FutureBass:
			ScoreBox->SetColorAndOpacity(FLinearColor(0.89f, 0.53f, 0.53f, 1.0f));
			break;
		case ESMTeam::Max:
			break;
	}

	switch (PlayerScoreData.CharacterType)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			CharacterType->SetText(LOCTABLE("Game", "Character_ElectricGuitar"));
			break;
		case ESMCharacterType::Piano:
			CharacterType->SetText(LOCTABLE("Game", "Character_Piano"));
			break;
		case ESMCharacterType::Bass:
			CharacterType->SetText(LOCTABLE("Game", "Character_Bass"));
			break;
	}

	PlayerName->SetText(FText::FromString(PlayerScoreData.PlayerName));
	if (bIsOwningPlayer)
	{
		PlayerName->SetColorAndOpacity(FLinearColor(0.36f, 0.96f, 0.28f, 1.0f));
	}

	Profile->SetProfileImage(PlayerScoreData.PlayerTeam, PlayerScoreData.CharacterType);
	Score->SetText(FText::AsNumber(PlayerScoreData.TotalScore()));

	Column1->SetText(FText::AsNumber(PlayerScoreData.TotalCapturedTiles));
	Column2->SetText(FText::AsNumber(PlayerScoreData.TotalDamageDealt));
	Column3->SetText(FText::AsNumber(PlayerScoreData.TotalKillCount));
	Column4->SetText(FText::AsNumber(PlayerScoreData.TotalDamageReceived));
	Column5->SetText(FText::AsNumber(PlayerScoreData.TotalDeathCount));
	Column6->SetText(FText::AsNumber(PlayerScoreData.TotalNoiseBreakUsage));
}
