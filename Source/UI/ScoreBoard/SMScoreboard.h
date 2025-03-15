// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "Data/SMTeam.h"
#include "SMScoreboard.generated.h"

class UOverlay;
class USMCharacterIllustration;
class UProgressBar;
class USMSimplePlayerScore;
class UImage;
class USMScoreboardAnalytics;
class UCommonTextBlock;
class USMPlayerScore;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMScoreboard : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void ShowScoreboard(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData);

protected:
	void SetCharacterIllustration(const TArray<FPlayerScoreData>& PlayerData) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Background;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ResultText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EDMScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> FBScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> EDMScoreProgress;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> FBScoreProgress;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterIllustration> CharacterIllustration1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterIllustration> CharacterIllustration2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMCharacterIllustration> CharacterIllustration3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> EDMMVP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> EDMACE;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FBMVP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FBACE;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMSimplePlayerScore> EDMPlayerScore1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMSimplePlayerScore> EDMPlayerScore2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMSimplePlayerScore> EDMPlayerScore3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMSimplePlayerScore> FBPlayerScore1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMSimplePlayerScore> FBPlayerScore2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMSimplePlayerScore> FBPlayerScore3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USMScoreboardAnalytics> ScoreboardAnalytics;

	TArray<FPlayerScoreData> EDMAnalyticsData;

	TArray<FPlayerScoreData> FBAnalyticsData;
};
