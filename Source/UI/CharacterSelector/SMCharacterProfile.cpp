// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterProfile.h"

#include "Components/Image.h"
#include "Data/SMCharacterType.h"

void USMCharacterProfile::SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType)
{
	for (const TObjectPtr<UImage>& Profile : { ProfileEDM1, ProfileEDM2, ProfileEDM3, ProfileFB1, ProfileFB2, ProfileFB3 })
	{
		Profile->SetVisibility(ESlateVisibility::Hidden);
	}

	switch (CharacterType)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			(Team == ESMTeam::EDM ? ProfileEDM1 : ProfileFB1)->SetVisibility(ESlateVisibility::Visible);
			break;
		case ESMCharacterType::Piano:
			(Team == ESMTeam::EDM ? ProfileEDM2 : ProfileFB2)->SetVisibility(ESlateVisibility::Visible);
			break;
		case ESMCharacterType::Bass:
			(Team == ESMTeam::EDM ? ProfileEDM3 : ProfileFB3)->SetVisibility(ESlateVisibility::Visible);
			break;
	}
}

void USMCharacterProfile::SetProfileImageScalar(float InScalar)
{
	for (auto& Profile : { ProfileEDM1, ProfileEDM2, ProfileEDM3, ProfileFB1, ProfileFB2, ProfileFB3 })
	{
		if (Profile && Profile->GetDynamicMaterial())
		{
			Profile->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), InScalar);
		}
	}
}
