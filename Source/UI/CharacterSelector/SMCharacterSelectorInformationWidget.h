// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Data/SMCharacterType.h"
#include "SMCharacterSelectorInformationWidget.generated.h"

class USMCharacterSelectorSkillInfo;
class ASMPlayerState;
class ASMCharacterSelectPlayerState;
class USMCharacterSelectorProfile;
class UImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorInformationWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ResetInfo() const;

	void ResetPlayerInfo() const;

	void SetPlayerInfo(const TArray<ASMCharacterSelectPlayerState*>& InPlayerArray, int32 InOwnerPlayerIndex);

	void SetSkillInfo(ESMCharacterType InPlayerCharacterTypes);

	void SetPlayerReady(int32 PlayerIndex, ESMCharacterType CharacterType, bool bIsReady, bool bIsPredicated = false);

protected:
	UPROPERTY()
	TArray<TObjectPtr<USMCharacterSelectorProfile>> PlayerProfiles;

	UPROPERTY()
	TArray<TObjectPtr<USMCharacterSelectorSkillInfo>> PlayerSkills;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SelectedCharacterType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterSelectorProfile> PlayerProfile1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterSelectorProfile> PlayerProfile2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterSelectorProfile> PlayerProfile3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterSelectorSkillInfo> AttackInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterSelectorSkillInfo> SkillInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterSelectorSkillInfo> NoiseBreakInfo;
};
