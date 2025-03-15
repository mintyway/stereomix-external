// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Skill.h"
#include "SMGA_ImpactArrow.generated.h"

class USMAT_SkillIndicator;
class UEnhancedInputComponent;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_ImpactArrow : public USMGA_Skill
{
	GENERATED_BODY()

public:
	USMGA_ImpactArrow();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/** 공격이 입력되면 호출됩니다. */
	void OnShoot();

	/** 서버에 발사를 요청합니다. */
	UFUNCTION(Server, Reliable)
	void ServerRequestShoot(const FVector_NetQuantize10& ImpactLocation);

	/** 서버에서만 작동합니다. 충격화살이 폭발할때 호출됩니다. */
	void OnImpact(const FVector& ImpactLocation);

	/** 충격화살에 피격될 액터들을 반환합니다. */
	TArray<AActor*> GetImpactHitActors(const FVector& ImpactLocation);

	UPROPERTY()
	TObjectPtr<USMAT_SkillIndicator> SkillIndicatorTask;

	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> ShootInputComponent;

	/** 충격화살이 떨어지기까지 선 딜레이입니다. */
	float ImpactPreDelay = 0.0f;

	/** 충격화살의 효과 범위입니다. */
	float Radius = 0.0f;

	/** 넉백의 힘입니다. */
	float KnockbackMagnitude = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseDebug:1 = false;
};
