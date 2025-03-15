// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_WaitHoldResult.h"

#include "Engine/OverlapResult.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


USMAT_WaitHoldResult* USMAT_WaitHoldResult::WaitHoldResult(UGameplayAbility* OwningAbility, const FVector& NewStartLocation, const FVector& NewCursorLocation, float NewMaxDistance, float NewLimitAngle, float UnconditionalHoldDistance)
{
	USMAT_WaitHoldResult* NewObj = NewAbilityTask<USMAT_WaitHoldResult>(OwningAbility);
	NewObj->SourceCharacter = Cast<ASMPlayerCharacterBase>(OwningAbility->GetAvatarActorFromActorInfo());
	NewObj->StartLocation = NewStartLocation;
	NewObj->CursorLocation = NewCursorLocation;
	NewObj->MaxDistance = NewMaxDistance;
	NewObj->LimitAngle = NewLimitAngle;
	NewObj->UnconditionalHoldDistance = UnconditionalHoldDistance;
	return NewObj;
}

void USMAT_WaitHoldResult::Activate()
{
	AActor* HeldActor = GetOptimalHoldableActor();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinish.ExecuteIfBound(HeldActor);
		EndTask();
	}
}

AActor* USMAT_WaitHoldResult::GetOptimalHoldableActor()
{
	if (!SourceCharacter.IsValid())
	{
		return nullptr;
	}

	TArray<AActor*> HoldableActors;
	TArray<FOverlapResult> OverlapResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxDistance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Hold), false, SourceCharacter.Get());
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::ActionTC, CollisionShape, Params))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* TargetActor = OverlapResult.GetActor();
			const USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
			if (!TargetHIC)
			{
				continue;
			}

			if (!TargetHIC->CanBeHeld(SourceCharacter.Get())) // 잡을 수 있는 상태인지 체크합니다.
			{
				continue;
			}

			HoldableActors.Add(TargetActor);
		}
	}

	// 커서 위치에 가장 가까운 액터를 반환합니다.
	return GetClosestActorFromLocation(HoldableActors, CursorLocation);
}

bool USMAT_WaitHoldResult::IsValidateAngle(const AActor* TargetActor)
{
	// 무조건 잡기 거리 이내에 타겟이 존재하면 각도 조건을 무시하고 유효한 각도로 취급합니다.
	const float DistanceSquared = FVector::DistSquared(StartLocation, TargetActor->GetActorLocation());
	if (DistanceSquared <= FMath::Square(UnconditionalHoldDistance))
	{
		return true;
	}

	// 각도 조건을 통해 잡을 수 있는지 없는지 추려냅니다.
	const FVector SourceToCursorDirection = (CursorLocation - StartLocation).GetSafeNormal();
	const FVector SourceToTargetDirection = (TargetActor->GetActorLocation() - StartLocation).GetSafeNormal();

	const float DotProduct = SourceToCursorDirection.Dot(SourceToTargetDirection);
	const float RadianDifference = FMath::Acos(DotProduct);

	// LimitDegree각도 내부라면 유효한 각도입니다.
	const float HalfLimitRadian = FMath::DegreesToRadians(LimitAngle / 2.0f);
	if (RadianDifference <= HalfLimitRadian)
	{
		return true;
	}

	return false;
}

AActor* USMAT_WaitHoldResult::GetClosestActorFromLocation(const TArray<AActor*>& TargetActors, const FVector& Location)
{
	AActor* ClosestActor = nullptr;
	float ClosestDistanceSquaredFromLocation = MAX_FLT;
	for (AActor* TargetActor : TargetActors)
	{
		if (!TargetActor)
		{
			continue;
		}

		const FVector TargetActorLocation = TargetActor->GetActorLocation();
		const float DistanceSquared = FVector::DistSquared(TargetActorLocation, Location);
		if (DistanceSquared < ClosestDistanceSquaredFromLocation)
		{
			ClosestActor = TargetActor;
			ClosestDistanceSquaredFromLocation = DistanceSquared;
		}
	}

	return ClosestActor;
}
