// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "Data/Enum/SMRoundState.h"
#include "SMCharacterLandingComponent.generated.h"

class USMRoundTimerManagerComponent;
class ASMPlayerCharacterBase;

UCLASS(DisplayName = "StereoMix Character Landing Component", meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCharacterLandingComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	USMCharacterLandingComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	// 서버 전용입니다. 캐릭터의 활성화 여부를 설정합니다.
	void SetCharacterActivation(bool bIsActivated);

protected:
	void OnASCInitialized();

	/** 카운트 다운이 시작될 때 호출됩니다. BindCountdownStart에 의해 호출됩니다. */
	void OnCountdownStarted();

	void OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState);
	
	void OnInRoundStarted();

	UPROPERTY()
	TObjectPtr<ASMPlayerCharacterBase> OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 CountdownTotalTime = 5;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SpawnInterval = 0.15f;

private:
	void BindGameState(AGameStateBase* InGameState);

	/** 로컬 전용입니다. 각 팀별 캐릭터의 레벨 시퀀스를 재생합니다. */
	void PlayCountdownLevelSequence();

	/** 로컬 전용입니다. 타이밍에 맞게 랜딩을 재생을 시작합니다. */
	void StartLanding();

	/** 랜딩을 재생합니다. */
	void PlayLanding();

	uint32 bIsStarted:1 = false;

	/** 컴파일용 bool 변수입니다. 에디터 버전에 한해 이 값에 따라 랜딩 활성화 여부가 선택됩니다. */
	uint32 bUseLandingInEditor:1 = false;
};
