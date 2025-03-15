// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "Data/Enum/SMRoundState.h"
#include "SMRoundComponent.generated.h"


enum class ESMTeam : uint8;
DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnRoundStateUpdated, ESMRoundState PreviouseRoundState, ESMRoundState NewRoundState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnRoundStateUpdateDynamic, ESMRoundState, PreviouseRoundState, ESMRoundState, NewRoundState);

DECLARE_MULTICAST_DELEGATE_OneParam(FSMOnRoundTimeUpdated, float UpdatedTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnRoundTimeUpdateDynamic, float, UpdatedTime);


UCLASS(Abstract, DisplayName = "StereoMix Round Component")
class STEREOMIX_API USMRoundComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	USMRoundComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "StereoMix|Round", DisplayName = "Get Round Manager", meta = (WorldContext = "WorldContextObject"))
	static USMRoundComponent* Get(const UObject* WorldContextObject);

	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ShouldShowLoadingScreen(FString& OutReason) override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	int32 GetRoundCount() const { return RoundCount; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	int32 GetActiveRound() const { return Round; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	int32 GetRoundScore(ESMTeam Team) const;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	ESMRoundState GetRoundState() const { return RoundState; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	ESMRoundState GetPreviousRoundState() const { return PreviousRoundState; }

	/** 현재 라운드의 진행 시간을 반환합니다. */
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	int32 GetActiveRoundTime() const { return RoundTime; }

	/** 현재 라운드의 남은 시간을 반환합니다. */
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	int32 GetRemainRoundTime() const { return RemainRoundTime; }

	/** 라운드를 시작합니다. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "StereoMix|Round")
	void ManualStartRound();

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Round")
	bool ShouldPlayNextRound() const;
	
	/** 라운드를 종료하고 PostRound 상태로 진입합니다. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "StereoMix|Round")
	void ManualEndRound();

	/** 다음 라운드로 이동하거나 결과창을 표시합니다. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "StereoMix|Round")
	void ManualFinishRound();

	FSMOnRoundStateUpdated OnRoundStateUpdatedEvent;
	FSMOnRoundTimeUpdated OnRemainRoundTimeUpdatedEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Round", DisplayName = "On Round State Updated")
	FSMOnRoundStateUpdateDynamic K2_OnRoundStateUpdatedEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Round", DisplayName = "On Round Time Updated")
	FSMOnRoundTimeUpdateDynamic K2_OnRemainRoundTimeUpdatedEvent;

	/** 활성화할 경우 PreRound 진입 후 자동으로 타이머 시작 후 InRound로 진입합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	bool bAutoStartRound = true;

	/** 활성화할 경우 PostRound 진입 후 자동으로 결과창 표시 또는 다음 맵으로 이동합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	bool bAutoEndRound = true;

	/**
	 * 현재 라운드의 시작 전 시간을 나타냅니다.
	 *
	 * 아래의 순서로 시간이 정해집니다.
	 *
	 * 1. 레벨 옵션에 "PreRoundTime="이 있으면 해당 값으로 설정
	 * 2. 프로퍼티 창에서 설정한 값으로 설정
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	float PreRoundTime = 6.0f;

	/**
	 * 현재 라운드의 진행 시간을 나타냅니다.
	 *
	 * 아래의 순서로 시간이 정해집니다.
	 *
	 * 1. 레벨 옵션에 "RoundTime="이 있으면 해당 값으로 설정
	 * 2. 프로퍼티 창에서 설정한 값으로 설정
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	float GameRoundTime = 60.0f;

	/**
	 * 결과 화면의 표시 시간을 나타냅니다. 해당 시간이 진행된 후에 라운드가 자동으로 종료됩니다.
	 *
	 * 아래의 순서로 시간이 정해집니다.
	 *
	 * 1. 레벨 옵션에 "ResultTime="이 있으면 해당 값으로 설정
	 * 2. 프로퍼티 창에서 설정한 값으로 설정
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	float ResultTime = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	float RoundTimerInterval = 1.0f;

protected:
	void InitializeRound();

	/**
	 * 라운드가 종료되었을 때 다음 맵으로 이동하기 위해 호출됩니다.
	 *
	 * 마지막 라운드일 때 커스텀매치가 아닌 경우에는 플레이어들이 알아서 나가야 하므로 맵을 이동하지 않고
	 * 커스텀매치라면 로비로 이동됩니다.
	 */
	void TravelToNextMap(int32 NextRound = -1);

	void StartRoundTimer(float InTime);
	void StopRoundTimer();

	void SetRoundState(ESMRoundState NewRoundState);
	void SetActiveRoundTime(float InTime);
	void SetRemainRoundTime(float InRemainTime);

	// void StartCharacterLandingOnLocal();

	void HandleGameReadyToStart();
	void HandleRoundTimerTick();
	void HandleRoundTimeEnd();

	void BeginLevelSequence();
	void BeginPreRound();
	void BeginInRound();
	void BeginPostRound();
	void BeginResult();

	void TravelToNextMapWhenAllPlayersReady();

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Round")
	FGameplayTagContainer LockTagsOnPreRound;

private:
	void BroadcastRoundStateUpdated(ESMRoundState InPreviousRoundState, ESMRoundState InNewRoundState) const;
	void BroadcastRoundTimeUpdated(float InUpdatedTime) const;

	void LockAbilities() const;
	void UnlockAbilities() const;

	UFUNCTION()
	void HandleVictoryTeamAnnounced(ESMTeam VictoryTeam);

	UFUNCTION()
	void OnRep_RoundState(ESMRoundState InPreviousRoundState);

	UFUNCTION()
	void OnRep_RoundTime();

	UFUNCTION()
	void OnRep_RemainRoundTime();

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = "StereoMix|Round")
	int32 RoundCount = -1;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = "StereoMix|Round")
	int32 Round = 0;

	// TODO: RoundScore를 배열이나 커스텀 구조체로 변경

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = "StereoMix|Round")
	int32 RoundScore_0;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = "StereoMix|Round")
	int32 RoundScore_1;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_RoundState, Category = "StereoMix|Round")
	ESMRoundState RoundState;

	UPROPERTY(VisibleInstanceOnly, Category = "StereoMix|Round")
	ESMRoundState PreviousRoundState;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_RoundTime, Category = "StereoMix|Round")
	float RoundTime = 0.0f;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_RemainRoundTime, Category = "StereoMix|Round")
	float RemainRoundTime = 0.0f;

	ESMTeam RoundWinner;
	bool bPendingNextMap = false;
	FTimerHandle RoundTimerHandle;
};
