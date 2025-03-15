// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_WaitHoldResult.generated.h"

class ASMPlayerCharacterBase;
DECLARE_DELEGATE_OneParam(FOnWaitHoldResultDelegate, AActor* /*HoldedAcotor*/);

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_WaitHoldResult : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_WaitHoldResult* WaitHoldResult(UGameplayAbility* OwningAbility, const FVector& NewStartLocation, const FVector& NewCursorLocation, float NewMaxDistance, float NewLimitAngle, float UnconditionalHoldDistance);

	TDelegate<void(AActor* /*HeldActor*/)> OnFinish;

protected:
	virtual void Activate() override;

	/** 잡을 수 있는 가장 적합한 타겟을 반환합니다.*/
	AActor* GetOptimalHoldableActor();

	/** 타겟이 유효한 각도 내에 존재하는 지 체크합니다. 현재는 사용하지 않습니다. */
	bool IsValidateAngle(const AActor* TargetActor);

	/** 지정된 위치에 가장 가까운 캐릭터를 얻어냅니다. */
	static AActor* GetClosestActorFromLocation(const TArray<AActor*>& TargetActors, const FVector& Location);

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	FVector StartLocation;

	FVector CursorLocation;

	float MaxDistance = 300.0f;

	float LimitAngle = 135.0f;

	float UnconditionalHoldDistance = 150.0f;
};
