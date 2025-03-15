// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_Charge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

USMAT_Charge* USMAT_Charge::ChargeStart(UGameplayAbility* OwningAbility)
{
	USMAT_Charge* MyObj = NewAbilityTask<USMAT_Charge>(OwningAbility);
	MyObj->OwnerCharacter = Cast<ASMBassCharacter>(OwningAbility->GetAvatarActorFromActorInfo());
	return MyObj;
}

void USMAT_Charge::Activate()
{
	if (!OwnerCharacter.Get())
	{
		EndTask();
		return;
	}

	if (ChargeColliderComponent = NewObject<UBoxComponent>(OwnerCharacter.Get()); ChargeColliderComponent)
	{
		ChargeColliderComponent->RegisterComponent();
		ChargeColliderComponent->AttachToComponent(OwnerCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		ChargeColliderComponent->SetCollisionProfileName(SMCollisionProfileName::Charge);
		ChargeColliderComponent->SetBoxExtent(FVector(50.0, 150.0, 150.0));
		ChargeColliderComponent->SetRelativeLocation(FVector(50.0, 0.0, 0.0));
		ChargeColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnChargeOverlappedCallback);
	}

	if (UCapsuleComponent* OwnerCapsuleComponent = OwnerCharacter.Get() ? OwnerCharacter->GetCapsuleComponent() : nullptr)
	{
		OwnerCapsuleComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnChargeBlockedCallback);
	}
}

void USMAT_Charge::OnDestroy(bool bInOwnerFinished)
{
	if (ChargeColliderComponent)
	{
		ChargeColliderComponent->OnComponentBeginOverlap.RemoveAll(this);
		ChargeColliderComponent->DestroyComponent();
		ChargeColliderComponent = nullptr;
	}

	if (UCapsuleComponent* OwnerCapsuleComponent = OwnerCharacter.Get() ? OwnerCharacter->GetCapsuleComponent() : nullptr)
	{
		OwnerCapsuleComponent->OnComponentHit.RemoveAll(this);
	}

	OnChargeBlocked.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_Charge::OnChargeOverlappedCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OwnerCharacter.Get() || !OtherActor)
	{
		return;
	}

	if (USMTeamBlueprintLibrary::IsSameTeam(OwnerCharacter.Get(), OtherActor))
	{
		return;
	}

	const ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(OtherActor);
	if (!TargetDamageInterface || TargetDamageInterface->CanIgnoreAttack() || TargetDamageInterface->IsObstacle())
	{
		return;
	}

	if (const UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if (TargetASC->HasMatchingGameplayTag(SMTags::Character::State::Bass::Charge)) // 만약 상대도 돌진중이라면 그대로 지나갑니다.
		{
			return;
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnChargeBlocked.ExecuteIfBound(OtherActor, OtherActor->GetActorLocation());
	}

	EndTask();
}

void USMAT_Charge::OnChargeBlockedCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnChargeBlocked.ExecuteIfBound(OtherActor, Hit.Location);
	}

	EndTask();
}
