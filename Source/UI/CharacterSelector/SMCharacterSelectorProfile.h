// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMCharacterSelectorProfile.generated.h"

class USMCharacterProfile;
class ASMPlayerState;
class UCommonTextBlock;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorProfile : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType, ESMCharacterType FocusCharacterType);

	void SetPlayerName(const FString& Name, bool bIsOwner);

	void SetCharacterType(ESMCharacterType CharacterType);

	void SetPlayerReady(bool bReady);

	void SetProfileImageScalar(float InScalar);

	void SetVisibilityByCharacterType(ESMTeam Team, ESMCharacterType InCharacterType, ESlateVisibility InVisibility, float InScalar);

	void ResetPlayerInfo();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterProfile> ProfileImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerCharacterType;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ReadyState;

private:
	uint32 bIsReady:1 = false;
};
