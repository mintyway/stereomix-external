// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Attack.h"
#include "SMGA_Slash.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Slash : public USMGA_Attack
{
	GENERATED_BODY()

public:
	USMGA_Slash();

	uint32 bCanInput:1 = false;
	uint32 bIsInput:1 = false;
	uint32 bCanProceedNextAction:1 = false;
	uint32 bIsLeftSlashNext:1 = true;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION()
	void OnSlashJudgeStartCallback(FGameplayEventData Payload);

	UFUNCTION()
	void CanInputCallback(FGameplayEventData Payload) { bCanInput = true; }

	UFUNCTION()
	void CanProceedNextActionCallback(FGameplayEventData Payload) { bCanProceedNextAction = true; }

	UFUNCTION()
	void LeftSlashNextCallback(FGameplayEventData Payload)
	{
		bIsLeftSlashNext = true;
		ServerSendSlashDirection(true);
	}

	UFUNCTION()
	void RightSlashNextCallback(FGameplayEventData Payload)
	{
		bIsLeftSlashNext = false;
		ServerSendSlashDirection(false);
	}

	UFUNCTION(Server, Reliable)
	void ServerSendSlashDirection(bool bNewIsLeftSlashNext);

	void OnNextActionProcced();

	UFUNCTION(Server, Reliable)
	void ServerRPCApplyCost();

	void OnSlashHit(AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void ServerRPCSlashHit(AActor* TargetActor);

	/** 베기의 전방기준 각도입니다. */
	float Angle = 0.0f;

	/** 베기를 수행하는 동안 소요될 시간입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float TotalSlashTime = 0.3f;

	/** 콜라이더의 회전을 보여줄지 여부입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bShowDebug:1 = false;
};
