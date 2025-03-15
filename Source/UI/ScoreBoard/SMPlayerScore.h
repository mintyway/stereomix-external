// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "SMPlayerScore.generated.h"

class UCommonTextBlock;
class USMCharacterProfile;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMPlayerScore : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerScore(const FPlayerScoreData& PlayerScoreData, bool bIsOwningPlayer) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterProfile> Profile;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ScoreBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CharacterType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Score;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Column1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Column2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Column3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Column4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Column5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Column6;
};
