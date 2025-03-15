// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_Charge.generated.h"

class UCapsuleComponent;
class UBoxComponent;
class ASMBassCharacter;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_Charge : public UAbilityTask
{
	GENERATED_BODY()

public:
	static USMAT_Charge* ChargeStart(UGameplayAbility* OwningAbility);

	TDelegate<void(AActor* /*TargetActor*/, const FVector& /*HitLocation*/)> OnChargeBlocked;

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION()
	void OnChargeOverlappedCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnChargeBlockedCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	TWeakObjectPtr<ASMBassCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UBoxComponent> ChargeColliderComponent;
};
