// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SMTeam.h"
#include "SMCharacterProfile.generated.h"

enum class ESMCharacterType : uint8;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterProfile : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType);

	void SetProfileImageScalar(float InScalar);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileEDM1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileEDM2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileEDM3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileFB1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileFB2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProfileFB3;
};
