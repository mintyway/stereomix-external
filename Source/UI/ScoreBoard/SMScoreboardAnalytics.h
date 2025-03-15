// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "SMScoreboardAnalytics.generated.h"

class USMTeamColorSet;
class USMPlayerScore;
class UCommonTextBlock;


UCLASS(Abstract)
class STEREOMIX_API USMScoreboardAnalytics : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void SetAnalyticsData(ESMTeam WinTeam, const TArray<FPlayerScoreData>& WinTeamData, const TArray<FPlayerScoreData>& LoseTeamData);

	UFUNCTION(BlueprintCallable)
	void Show();

	UFUNCTION(BlueprintCallable)
	void Hide();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USMTeamColorSet> TeamColorSet;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowAnalytics;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HideAnalytics;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TeamName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TeamName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMPlayerScore> PlayerScore1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMPlayerScore> PlayerScore2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMPlayerScore> PlayerScore3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMPlayerScore> PlayerScore4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMPlayerScore> PlayerScore5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMPlayerScore> PlayerScore6;
};
