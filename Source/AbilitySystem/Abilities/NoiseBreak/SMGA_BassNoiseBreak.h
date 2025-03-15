// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_NoiseBreak.h"
#include "SMGA_BassNoiseBreak.generated.h"

class UAnimMontage;
class UGameplayEffect;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_BassNoiseBreak : public USMGA_NoiseBreak
{
	GENERATED_BODY()

public:
	USMGA_BassNoiseBreak();

	FVector GetNoiseBreakTargetLocation() const { return NoiseBreakTargetLocation; }

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	/** 도약 전 딜레이 타이머 종료시 호출됩니다. */
	void OnPreLeapTimerCallback(FVector EndLocation);

	/** 정점 높이를 기준으로 타겟을 향하는 벨로시티를 통해 캐릭터를 도약 시킵니다. */
	void LeapCharacter(const FVector& TargetLocation);

	UFUNCTION(Server, Reliable)
	void ServerSendNoiseBreakData(const FVector_NetQuantize10& TargetLocation);

	/** 매치기의 핵심 로직입니다. 착지 시 호출됩니다. */
	UFUNCTION()
	void OnLeapEnded();

	/** 싱크를 동기화하고 노이즈 브레이크를 종료합니다. */
	UFUNCTION()
	void OnNoiseBreakEndedCallback(FGameplayEventData Payload);

	TArray<ASMTile*> GetTilesToBeCaptured();

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNoiseBreakMontage;

	FVector NoiseBreakTargetLocation;

	FName OriginalCollisionProfileName;
};
