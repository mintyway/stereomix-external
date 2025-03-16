// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Skill.h"
#include "SMGA_SlowBullet.generated.h"

class USMAT_SkillIndicator;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_SlowBullet : public USMGA_Skill
{
	GENERATED_BODY()

public:
	USMGA_SlowBullet();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnShoot();

	UFUNCTION(Server, Reliable)
	void ServerRPCLaunchProjectile(const FVector_NetQuantize10& InSourceLocation, const FVector_NetQuantize10& InTargetLocation);

	UFUNCTION()
	void SyncPointEndAbility();

	UFUNCTION()
	void OnProjectileHit(AActor* HitActor);

	UPROPERTY(EditAnywhere, Category = "Design")
	float PreDelay = 0.5f;

	float ProjectileSpeed = 0.0f;

	/** 대상에게 적용할 이동속도 디버프 배율입니다. 0.3이면 0.3의 이동속도 즉, 70%의 이동속도가 감소합니다. */
	float SlowDebuffMultiplier = 0.0f;

	/** 디버프의 지속 시간입니다. */
	float SlowDebuffDuration = 0.0f;

	FVector SourceLocation;

	FVector TargetLocation;

	UPROPERTY()
	TObjectPtr<USMAT_SkillIndicator> SkillIndicatorTask;
};
