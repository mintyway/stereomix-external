// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorProfile.h"

#include "CommonTextBlock.h"
#include "SMCharacterProfile.h"
#include "Components/Image.h"
#include "Internationalization/StringTableRegistry.h"

class ASMPlayerState;

void USMCharacterSelectorProfile::SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType, ESMCharacterType FocusCharacterType)
{
	constexpr float EnabledScalar = 0.0f;
	constexpr float DisabledScalar = 2.0f;

	// 모든 프로필 비활성화
	for (auto& Profile : { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass })
	{
		SetVisibilityByCharacterType(Team, Profile, ESlateVisibility::Hidden, DisabledScalar);
	}

	if (FocusCharacterType == ESMCharacterType::None)
	{
		return;
	}

	SetCharacterType(CharacterType);

	// 선택된 상태에서 포커스를 변경한 경우
	if (CharacterType == ESMCharacterType::None || CharacterType != FocusCharacterType)
	{
		SetPlayerReady(false);
		SetVisibilityByCharacterType(Team, FocusCharacterType, ESlateVisibility::Visible, DisabledScalar);
		return;
	}

	// 선택된 캐릭터 프로필 활성화
	SetPlayerReady(CharacterType != ESMCharacterType::None);
	SetVisibilityByCharacterType(Team, CharacterType, ESlateVisibility::Visible, EnabledScalar);
}

void USMCharacterSelectorProfile::SetPlayerName(const FString& Name, const bool bIsOwner)
{
	PlayerName->SetText(FText::FromString(Name));
	PlayerName->SetColorAndOpacity(bIsOwner ? FLinearColor(FLinearColor(0.283149f, 0.996078f, 0.391573f, 1.0f)) : FLinearColor::White);
}

void USMCharacterSelectorProfile::SetCharacterType(const ESMCharacterType CharacterType)
{
	FText CharacterTypeText;
	switch (CharacterType)
	{
		case ESMCharacterType::None:
			CharacterTypeText = LOCTABLE("UI", "CharacterSelector_Selecting");
			break;
		case ESMCharacterType::ElectricGuitar:
			CharacterTypeText = LOCTABLE("Game", "Character_ElectricGuitar");
			break;
		case ESMCharacterType::Piano:
			CharacterTypeText = LOCTABLE("Game", "Character_Piano");
			break;
		case ESMCharacterType::Bass:
			CharacterTypeText = LOCTABLE("Game", "Character_Bass");
			break;
	}
	PlayerCharacterType->SetText(CharacterTypeText);
}

void USMCharacterSelectorProfile::SetPlayerReady(bool bReady)
{
	if (bIsReady == bReady)
	{
		return;
	}
	bIsReady = bReady;
	if (bIsReady)
	{
		PlayAnimationForward(ReadyState);
	}
	else
	{
		PlayAnimationReverse(ReadyState);
	}
}

void USMCharacterSelectorProfile::SetProfileImageScalar(float InScalar)
{
	ProfileImage->SetProfileImageScalar(InScalar);
}

void USMCharacterSelectorProfile::SetVisibilityByCharacterType(ESMTeam Team, ESMCharacterType InCharacterType, ESlateVisibility InVisibility, float InScalar)
{
	ProfileImage->SetProfileImageScalar(InScalar);
	if (InVisibility == ESlateVisibility::Visible)
	{
		ProfileImage->SetProfileImage(Team, InCharacterType);
	}
	else
	{
		ProfileImage->SetProfileImage(ESMTeam::None, ESMCharacterType::None);
	}
}

void USMCharacterSelectorProfile::ResetPlayerInfo()
{
	SetPlayerReady(false);
	SetPlayerName("", false);
	PlayerCharacterType->SetText(FText::GetEmpty());
	ProfileImage->SetProfileImage(ESMTeam::None, ESMCharacterType::None);
}
