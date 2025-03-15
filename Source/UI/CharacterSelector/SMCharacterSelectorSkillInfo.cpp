// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorSkillInfo.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Internationalization/StringTableRegistry.h"

void USMCharacterSelectorSkillInfo::SetSkillInfo(ESMCharacterType CharacterType, int32 SkillType) const
{
	TArray<FText> InSkillNames;
	TArray<FText> InSkillDescriptions;

	ShortSkillType->SetVisibility(ESlateVisibility::Hidden);
	LongSkillType->SetVisibility(ESlateVisibility::Hidden);

	AttackIcon->SetVisibility(ESlateVisibility::Hidden);
	SkillIcon->SetVisibility(ESlateVisibility::Hidden);

	ElectricGuitarAttackIcon->SetVisibility(ESlateVisibility::Hidden);
	ElectricGuitarSkillIcon->SetVisibility(ESlateVisibility::Hidden);

	PianoAttackIcon->SetVisibility(ESlateVisibility::Hidden);
	PianoSkillIcon->SetVisibility(ESlateVisibility::Hidden);

	BassAttackIcon->SetVisibility(ESlateVisibility::Hidden);
	BassSkillIcon->SetVisibility(ESlateVisibility::Hidden);

	NoiseBreakIcon->SetVisibility(ESlateVisibility::Hidden);

	switch (SkillType)
	{
		case 0:
			AttackIcon->SetVisibility(ESlateVisibility::Visible);
			ShortSkillTypeText->SetText(FText::FromString(TEXT("일반공격")));
			ShortSkillType->SetVisibility(ESlateVisibility::Visible);
			break;
		case 1:
			SkillIcon->SetVisibility(ESlateVisibility::Visible);
			ShortSkillTypeText->SetText(FText::FromString(TEXT("스킬")));
			ShortSkillType->SetVisibility(ESlateVisibility::Visible);
			break;
		case 2:
			NoiseBreakIcon->SetVisibility(ESlateVisibility::Visible);
			LongSkillTypeText->SetText(FText::FromString(TEXT("노이즈 브레이크")));
			LongSkillType->SetVisibility(ESlateVisibility::Visible);
			break;
		default:
			break;
	}

	switch (CharacterType)
	{
		case ESMCharacterType::None:
			for (int32 i = 0; i < 3; ++i)
			{
				InSkillNames.Add(FText::GetEmpty());
				InSkillDescriptions.Add(FText::GetEmpty());
			}
			break;
		case ESMCharacterType::ElectricGuitar:
			ElectricGuitarAttackIcon->SetVisibility(ESlateVisibility::Visible);
			ElectricGuitarSkillIcon->SetVisibility(ESlateVisibility::Visible);
			InSkillNames.Add(LOCTABLE("Game", "Character_ElectricGuitar_Attack"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_ElectricGuitar_Attack_Desc"));
			InSkillNames.Add(LOCTABLE("Game", "Character_ElectricGuitar_Skill"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_ElectricGuitar_Skill_Desc"));
			InSkillNames.Add(LOCTABLE("Game", "Character_ElectricGuitar_NoiseBreak"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_ElectricGuitar_NoiseBreak_Desc"));
			break;
		case ESMCharacterType::Piano:
			PianoAttackIcon->SetVisibility(ESlateVisibility::Visible);
			PianoSkillIcon->SetVisibility(ESlateVisibility::Visible);
			InSkillNames.Add(LOCTABLE("Game", "Character_Piano_Attack"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_Piano_Attack_Desc"));
			InSkillNames.Add(LOCTABLE("Game", "Character_Piano_Skill"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_Piano_Skill_Desc"));
			InSkillNames.Add(LOCTABLE("Game", "Character_Piano_NoiseBreak"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_Piano_NoiseBreak_Desc"));
			break;
		case ESMCharacterType::Bass:
			BassAttackIcon->SetVisibility(ESlateVisibility::Visible);
			BassSkillIcon->SetVisibility(ESlateVisibility::Visible);
			InSkillNames.Add(LOCTABLE("Game", "Character_Bass_Attack"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_Bass_Attack_Desc"));
			InSkillNames.Add(LOCTABLE("Game", "Character_Bass_Skill"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_Bass_Skill_Desc"));
			InSkillNames.Add(LOCTABLE("Game", "Character_Bass_NoiseBreak"));
			InSkillDescriptions.Add(LOCTABLE("Game", "Character_Bass_NoiseBreak_Desc"));
			break;
	}

	Name->SetText(InSkillNames[SkillType]);
	Description->SetText(InSkillDescriptions[SkillType]);
}
