// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorTypeDisplayWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Internationalization/StringTableRegistry.h"

void USMCharacterSelectorTypeDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResetCharacterType();
	Ready->SetVisibility(ESlateVisibility::Hidden);
	Unready->SetVisibility(ESlateVisibility::Visible);
}

void USMCharacterSelectorTypeDisplayWidget::ResetCharacterType() const
{
	if (CharacterType)
	{
		CharacterType->SetText(FText::GetEmpty());
	}
}

void USMCharacterSelectorTypeDisplayWidget::SetCharacterType(ESMCharacterType InType) const
{
	ResetCharacterType();

	switch (InType)
	{
		case ESMCharacterType::None:
			Ready->SetVisibility(ESlateVisibility::Hidden);
			Unready->SetVisibility(ESlateVisibility::Visible);
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
		default:
			break;
	}
}

void USMCharacterSelectorTypeDisplayWidget::SetReady(bool bReady) const
{
	Ready->SetVisibility(bReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Unready->SetVisibility(bReady ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}
