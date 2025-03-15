// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterIllustration.h"

#include "Components/Image.h"
#include "Data/SMCharacterType.h"

void USMCharacterIllustration::SetCharacterIllustration(ESMTeam Team, ESMCharacterType CharacterType)
{
	auto SetImagesVisibility = [&](TArray<UImage*>& Images, bool bVisible) {
		for (int32 i = 0; i < Images.Num(); ++i)
		{
			Images[i]->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	};

	TArray<UImage*> TargetImages = {};

	TargetImages = { EDM1, EDM2, EDM3, FB1, FB2, FB3 };
	SetImagesVisibility(TargetImages, false);

	switch (CharacterType)
	{
		case ESMCharacterType::ElectricGuitar:
			TargetImages = { EDM1, FB1 };
			SetImagesVisibility(TargetImages, true);
			break;
		case ESMCharacterType::Piano:
			TargetImages = { EDM2, FB2 };
			SetImagesVisibility(TargetImages, true);
			break;
		case ESMCharacterType::Bass:
			TargetImages = { EDM3, FB3 };
			SetImagesVisibility(TargetImages, true);
			break;
		default:
			break;
	}

	switch (Team)
	{
		case ESMTeam::EDM:
			TargetImages = { FB1, FB2, FB3 };
			SetImagesVisibility(TargetImages, false);
			break;
		case ESMTeam::FutureBass:
			TargetImages = { EDM1, EDM2, EDM3 };
			SetImagesVisibility(TargetImages, false);
			break;
		default:
			break;
	}
}
