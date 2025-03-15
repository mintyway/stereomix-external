// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SMCharacterType.h"
#include "SMCharacterSelectorSkillInfo.generated.h"

class UOverlay;
class UCommonTextBlock;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorSkillInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSkillInfo(ESMCharacterType CharacterType, int32 SkillType) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ShortSkillTypeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LongSkillTypeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ShortSkillType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> LongSkillType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> AttackIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ElectricGuitarAttackIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ElectricGuitarSkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PianoAttackIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PianoSkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BassAttackIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BassSkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NoiseBreakIcon;
};
