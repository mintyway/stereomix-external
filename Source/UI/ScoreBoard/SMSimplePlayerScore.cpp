// Copyright Studio Surround. All Rights Reserved.


#include "SMSimplePlayerScore.h"

#include "CommonTextBlock.h"
#include "UI/CharacterSelector/SMCharacterProfile.h"

void USMSimplePlayerScore::SetPlayerScore(const FPlayerScoreData& PlayerScoreData, bool bIsOwningPlayer) const
{
	PlayerName->SetText(FText::FromString(PlayerScoreData.PlayerName));
	if (bIsOwningPlayer)
	{
		PlayerName->SetColorAndOpacity(FLinearColor(0.36f, 0.96f, 0.28f, 1.0f));
	}

	Profile->SetProfileImage(PlayerScoreData.PlayerTeam, PlayerScoreData.CharacterType);
	Score->SetText(FText::AsNumber(PlayerScoreData.TotalScore()));
}
