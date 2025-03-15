// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "SMSimplePlayerScore.generated.h"

class UCommonTextBlock;
class USMCharacterProfile;


UCLASS(Abstract)
class STEREOMIX_API USMSimplePlayerScore : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerScore(const FPlayerScoreData& PlayerScoreData, bool bIsOwningPlayer) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterProfile> Profile;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Score;
};
