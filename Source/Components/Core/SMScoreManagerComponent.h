// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameStateComponent.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "Data/Enum/SMRoundState.h"
#include "SMScoreManagerComponent.generated.h"


class ASMPlayerState;
class USMPlaylist;
class USMScoreboard;
class ASMPlayerCharacterBase;

USTRUCT(BlueprintType)
struct FPlayerScoreData
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<ASMPlayerState> PlayerState;

	UPROPERTY()
	ESMTeam PlayerTeam = ESMTeam::None;

	UPROPERTY()
	ESMCharacterType CharacterType = ESMCharacterType::None;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int32 TotalCapturedTiles = 0;

	UPROPERTY()
	float TotalDamageDealt = 0.0f;

	UPROPERTY()
	float TotalDamageReceived = 0.0f;

	UPROPERTY()
	int32 TotalKillCount = 0;

	UPROPERTY()
	int32 TotalDeathCount = 0;

	UPROPERTY()
	int32 TotalNoiseBreakUsage = 0;

	UPROPERTY()
	float CorrectionValue = 0.4f;

	int32 TotalScore() const
	{
		const int32 KillScore = TotalKillCount == 0 ? 1 : TotalKillCount * CorrectionValue;
		const float DamageDealtScore = TotalDamageDealt == 0.0f ? KillScore : TotalDamageDealt + (KillScore * 1.5f);
		const int32 NoiseBreakUsageScore = TotalNoiseBreakUsage == 0 ? 1 : (TotalNoiseBreakUsage + (TotalNoiseBreakUsage * CorrectionValue)) * 10;
		const int32 CapturedTilesScore = TotalCapturedTiles + (TotalCapturedTiles * CorrectionValue);
		return CapturedTilesScore + DamageDealtScore + NoiseBreakUsageScore;
	}

	FPlayerScoreData operator+(const FPlayerScoreData& Other) const
	{
		FPlayerScoreData Result;

		Result.PlayerTeam = PlayerTeam;
		Result.CharacterType = CharacterType;
		Result.PlayerName = PlayerName;
		Result.TotalCapturedTiles = TotalCapturedTiles + Other.TotalCapturedTiles;
		Result.TotalDamageDealt = TotalDamageDealt + Other.TotalDamageDealt;
		Result.TotalDamageReceived = TotalDamageReceived + Other.TotalDamageReceived;
		Result.TotalKillCount = TotalKillCount + Other.TotalKillCount;
		Result.TotalDeathCount = TotalDeathCount + Other.TotalDeathCount;
		Result.TotalNoiseBreakUsage = TotalNoiseBreakUsage + Other.TotalNoiseBreakUsage;
		Result.CorrectionValue = CorrectionValue;

		return Result;
	}
};

UCLASS(meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMScoreManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	USMScoreManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static USMScoreManagerComponent* Get(const UObject* WorldContextObject);

	virtual void BeginPlay() override;

	UFUNCTION()
	void AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount);

	UFUNCTION()
	void AddTotalDamageDealt(const AActor* TargetPlayer, float DamageDealt);

	UFUNCTION()
	void AddTotalDamageReceived(const AActor* TargetPlayer, float DamageReceived);

	UFUNCTION()
	void AddTotalDeathCount(const AActor* TargetPlayer);

	UFUNCTION()
	void AddTotalKillCount(const AActor* TargetPlayer);

	UFUNCTION()
	void AddTotalNoiseBreakUsage(const AActor* TargetPlayer);

	FPlayerScoreData GetPlayerScoreData(const ASMPlayerState* TargetPlayerState) const;

	TArray<FPlayerScoreData> GetTeamScoreData(ESMTeam Team);

	UFUNCTION()
	void LogAllPlayerData();

	UFUNCTION()
	void OnVictoryTeamAnnouncedCallback(ESMTeam VictoryTeam);

	void ShowResultToAllPlayers(ESMTeam VictoryTeam);

protected:
	void OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowScoreboard(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData);

	UPROPERTY(EditAnywhere, Category = "Design|UI|HUD")
	TSubclassOf<USMScoreboard> ScoreboardWidgetClass;

	UPROPERTY()
	TObjectPtr<USMScoreboard> ScoreboardWidget;

	static FPlayerScoreData GetDefaultPlayerScoreData(const TWeakObjectPtr<const AActor>& TargetPlayer);

	void LogPlayerData(const TWeakObjectPtr<const AActor>& TargetPlayer) const;

	TMap<TWeakObjectPtr<const AActor>, FPlayerScoreData> PlayerScoreData;
};
