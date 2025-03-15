// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Data/SMCharacterType.h"
#include "SMCharacterSelectorTypeDisplayWidget.generated.h"

class UCommonTextBlock;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorTypeDisplayWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ResetCharacterType() const;

	void SetCharacterType(ESMCharacterType InType) const;

	void SetReady(bool bReady) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CharacterType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Ready;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Unready;
};
