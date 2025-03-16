// Copyright Studio Surround. All Rights Reserved.


#include "SMEffectProjectileBase.h"

#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Interfaces/SMDamageInterface.h"


void ASMEffectProjectileBase::MulticastEndLifeTimeInternal_Implementation()
{
	OnProjectileHit.Unbind();
	Super::MulticastEndLifeTimeInternal_Implementation();
}

bool ASMEffectProjectileBase::IsValidTarget(AActor* InTarget)
{
	if (!Super::IsValidTarget(InTarget))
	{
		return false;
	}

	const ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	if (!TargetDamageInterface || TargetDamageInterface->CanIgnoreAttack())
	{
		return false;
	}

	if (USMTeamBlueprintLibrary::IsSameTeam(this, InTarget)) // 투사체와 타겟이 같은 팀이라면 무시합니다. (팀킬 방지)
	{
		return false;
	}

	return true;
}

void ASMEffectProjectileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority())
	{
		// 유효한 타겟인지 검증합니다.
		if (!IsValidTarget(OtherActor))
		{
			return;
		}

		HandleHitEffect(OtherActor);
		(void)OnProjectileHit.ExecuteIfBound(OtherActor);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
}

void ASMEffectProjectileBase::HandleHitEffect(AActor* InTarget)
{
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(InTarget);
	if (TargetDamageInterface ? !TargetDamageInterface->IsObstacle() : false)
	{
		PlayHitFX(InTarget);
	}
}

void ASMEffectProjectileBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (HasAuthority())
	{
		HandleWallHitEffect(HitLocation);

		// 투사체로서 할일을 다 했기에 투사체 풀로 돌아갑니다.
		EndLifeTime();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASMEffectProjectileBase::HandleWallHitEffect(const FVector& HitLocation)
{
	PlayWallHitFX(HitLocation);
}
