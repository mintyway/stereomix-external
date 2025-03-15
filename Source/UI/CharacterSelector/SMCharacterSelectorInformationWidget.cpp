// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorInformationWidget.h"

#include "CommonTextBlock.h"
#include "SMCharacterSelectorProfile.h"
#include "SMCharacterSelectorSkillInfo.h"
#include "Data/SMCharacterType.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "Internationalization/StringTableRegistry.h"

void USMCharacterSelectorInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerProfiles.Add(PlayerProfile1);
	PlayerProfiles.Add(PlayerProfile2);
	PlayerProfiles.Add(PlayerProfile3);

	PlayerSkills.Add(AttackInfo);
	PlayerSkills.Add(SkillInfo);
	PlayerSkills.Add(NoiseBreakInfo);

	ResetInfo();
}

void USMCharacterSelectorInformationWidget::ResetInfo() const
{
	SelectedCharacterType->SetText(FText::GetEmpty());

	for (int32 i = 0; i < 3; ++i)
	{
		if (PlayerProfiles.IsValidIndex(i))
		{
			PlayerProfiles[i]->ResetPlayerInfo();
		}

		if (PlayerSkills.IsValidIndex(i))
		{
			PlayerSkills[i]->SetSkillInfo(ESMCharacterType::None, i);
		}
	}
}

void USMCharacterSelectorInformationWidget::ResetPlayerInfo() const
{
	for (int32 i = 0; i < 3; ++i)
	{
		if (PlayerProfiles.IsValidIndex(i))
		{
			PlayerProfiles[i]->ResetPlayerInfo();
		}
	}
}

void USMCharacterSelectorInformationWidget::SetPlayerInfo(const TArray<ASMCharacterSelectPlayerState*>& InPlayerArray, int32 InOwnerPlayerIndex)
{
	for (int32 i = 0; i < PlayerProfiles.Num(); ++i)
	{
		if (InPlayerArray.IsValidIndex(i))
		{
			PlayerProfiles[i]->SetPlayerName(InPlayerArray[i]->GetPlayerName(), InOwnerPlayerIndex == i);
			PlayerProfiles[i]->SetProfileImage(InPlayerArray[i]->GetTeam(), InPlayerArray[i]->GetCharacterType(), InPlayerArray[i]->GetFocusCharacterType());
		}
		else
		{
			PlayerProfiles[i]->ResetPlayerInfo();
		}
	}
}

void USMCharacterSelectorInformationWidget::SetSkillInfo(const ESMCharacterType InPlayerCharacterTypes)
{
	switch (InPlayerCharacterTypes)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			SelectedCharacterType->SetText(LOCTABLE("Game", "Character_ElectricGuitar"));
			break;
		case ESMCharacterType::Piano:
			SelectedCharacterType->SetText(LOCTABLE("Game", "Character_Piano"));
			break;
		case ESMCharacterType::Bass:
			SelectedCharacterType->SetText(LOCTABLE("Game", "Character_Bass"));
			break;
	}

	for (int32 i = 0; i < PlayerSkills.Num(); ++i)
	{
		if (PlayerSkills.IsValidIndex(i))
		{
			PlayerSkills[i]->SetSkillInfo(InPlayerCharacterTypes, i);
		}
	}
}

void USMCharacterSelectorInformationWidget::SetPlayerReady(const int32 PlayerIndex, const ESMCharacterType CharacterType, const bool bIsReady, const bool bIsPredicated)
{
	if (bIsPredicated)
	{
		constexpr float EnabledScalar = 0.0f;
		constexpr float DisabledScalar = 2.0f;
		PlayerProfiles[PlayerIndex]->SetProfileImageScalar(bIsReady ? EnabledScalar : DisabledScalar);
		PlayerProfiles[PlayerIndex]->SetCharacterType(bIsReady ? CharacterType : ESMCharacterType::None);
	}
	PlayerProfiles[PlayerIndex]->SetPlayerReady(bIsReady);
}
