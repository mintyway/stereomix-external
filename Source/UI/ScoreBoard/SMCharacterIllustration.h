// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMCharacterIllustration.generated.h"

class UImage;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterIllustration : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCharacterIllustration(ESMTeam Team, ESMCharacterType CharacterType);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> EDM1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> EDM2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> EDM3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> FB1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> FB2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> FB3;
};
