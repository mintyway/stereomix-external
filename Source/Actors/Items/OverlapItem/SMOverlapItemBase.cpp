// Copyright Studio Surround. All Rights Reserved.


#include "SMOverlapItemBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMOverlapItemBase::ASMOverlapItemBase()
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	UnavailableTags.AddTag(SMTags::Character::State::Common::Immune);
	UnavailableTags.AddTag(SMTags::Character::State::Common::Neutralized);
}

void ASMOverlapItemBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (HasAuthority() && World)
	{
		// 아이템이 즉시 먹어지는 것을 방지하고자 아이템 생성 후 0.5초뒤에 활성화 되도록합니다.
		TWeakObjectPtr<USphereComponent> ColliderWeakPtr(ColliderComponent);
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ColliderWeakPtr] {
			if (ColliderWeakPtr.IsValid())
			{
				ColliderWeakPtr->SetCollisionProfileName(SMCollisionProfileName::OverlapItem);
			}
		}, ActivateDelay, false);
	}
}

void ASMOverlapItemBase::DeactivateItem()
{
	if (OnDestroyFX)
	{
		MulticastPlayNiagara(OnDestroyFX, NiagaraComponent->GetComponentLocation());
	}

	Super::DeactivateItem();
}

void ASMOverlapItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UAbilitySystemComponent* ActivatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!ActivatorASC || ActivatorASC->HasAnyMatchingGameplayTags(UnavailableTags))
	{
		return;
	}

	MulticastPlayNiagara(OnActivateFX, NiagaraComponent->GetComponentLocation());

	NET_LOG(this, Warning, TEXT("%s가 아이템을 획득 했습니다."), *GetNameSafe(OtherActor));
	ActivateItem(OtherActor);

	Destroy();
}
