// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Hold.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_WaitHoldResult.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"

USMGA_Hold::USMGA_Hold()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags.AddTag(SMTags::Ability::Activation::Hold);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Blocking::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Held);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Neutralized);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Immune);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Bass::Charge);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Piano::ImpactArrow);
}

void USMGA_Hold::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	USMAbilitySystemComponent* OwnerASC = GetASC();
	const USMPlayerCharacterDataAsset* OwnerDataAsset = GetDataAsset();
	if (!OwnerCharacter || !OwnerASC || !OwnerDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// 언제든 다른 상태로 인해 끊길 수 있는 애니메이션이기에 bAllowInterruptAfterBlendOut을 활성화 해 언제 끊기던 OnInterrupted가 브로드 캐스트 되도록합니다.
	// 클라이언트와 서버 각각 애니메이션이 종료되면 스스로 종료하도록 합니다.
	if (UAnimInstance* OwnerAnimInstance = ActorInfo->GetAnimInstance())
	{
		UAnimMontage* HoldMontage = OwnerDataAsset->HoldMontage[OwnerCharacter->GetTeam()];
		OwnerASC->PlayMontage(this, ActivationInfo, HoldMontage, 1.0f);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindWeakLambda(this, [this](UAnimMontage*, bool) { K2_EndAbility(); });
		OwnerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate);
	}

	if (IsLocallyControlled())
	{
		// 커서위치는 즉시 저장합니다. 시작 위치는 ServerRPC를 호출할때 저장합니다.
		CursorLocation = OwnerCharacter->GetLocationFromCursor();

		// 애님 노티파이를 기다리고 노티파이가 호출되면 잡기를 요청합니다.
		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Common::Hold);
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnHoldAnimNotifyTrigger);
		WaitGameplayEventTask->ReadyForActivation();

		// 잡기 시전 시 이펙트입니다.
		FGameplayCueParameters GCParams = MakeGameplayCueParametersAttached(OwnerCharacter, FVector::ZeroVector, FVector::ZeroVector);
		OwnerASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Common::Hold, GCParams);
	}
}

void USMGA_Hold::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bSuccessHold = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Hold::OnHoldAnimNotifyTrigger(FGameplayEventData Payload)
{
	const ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	if (!OwnerCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	// 시전 위치를 저장하고 서버에 잡기 요청을 보냅니다.
	StartLocation = OwnerCharacter->GetActorLocation();
	ServerRPCRequestHold(StartLocation, CursorLocation);
}

void USMGA_Hold::ServerRPCRequestHold_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	StartLocation = InStartLocation;
	CursorLocation = InCursorLocation;

	USMAT_WaitHoldResult* HoldResultTask = USMAT_WaitHoldResult::WaitHoldResult(this, StartLocation, CursorLocation, MaxDistance, LimitAngle, UnconditionalHoldDistance);
	HoldResultTask->OnFinish.BindUObject(this, &ThisClass::OnHold);
	HoldResultTask->ReadyForActivation();
}

void USMGA_Hold::OnHold(AActor* TargetActor)
{
	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	USMHIC_Character* OwnerHIC = GetHIC();
	if (!OwnerCharacter || !OwnerHIC)
	{
		EndAbilityByCancel();
		return;
	}

	bool bSuccess = false;
	if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
	{
		const FVector PreHeldLocation = TargetActor->GetActorLocation(); // 잡히기 이전 위치를 저장합니다. 이펙트에 사용됩니다.

		TargetHIC->OnHeld(OwnerCharacter); // 타겟의 잡히기 로직을 실행합니다.

		// 잡은 대상을 저장하고 자신의 상태를 잡기로 변경합니다. 예외처리를 위해 잡은 액터의 파괴 시 수행해야할 이벤트를 바인드합니다.
		OwnerHIC->SetActorIAmHolding(TargetActor);
		TargetActor->OnDestroyed.AddDynamic(OwnerHIC, &USMHIC_Character::OnDestroyedIAmHoldingActor);

		OwnerCharacter->ClientRemoveScreenIndicatorToSelf(TargetActor); // 로컬에서 타겟 인디케이터를 제거합니다.

		if (const USMAbilitySystemComponent* SourceASC = GetASC())
		{
			const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
			const FVector OwnerToTargetDirection = (PreHeldLocation - OwnerLocation).GetSafeNormal();

			// 잡기 적중에 성공하여 성공 이펙트를 재생합니다.
			FGameplayCueParameters SuccessGCParams = MakeGameplayCueParametersAttached(OwnerCharacter, FVector::ZeroVector, OwnerToTargetDirection);
			SourceASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Common::HoldSuccess, SuccessGCParams);

			// 대상위치에 적중 이펙트를 재생합니다.
			FGameplayCueParameters HitGCParams = MakeGameplayCueParameters(PreHeldLocation, OwnerToTargetDirection);
			SourceASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Common::HoldHit, HitGCParams);
		}

		bSuccess = true;
		OnHoldSucceed.Broadcast();
	}

	bSuccessHold = bSuccess;
	ClientRPCSendHoldResult(bSuccessHold);

	if (bShowDebug)
	{
		const FColor Color = bSuccessHold ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), StartLocation, MaxDistance, 16, Color, false, 2.0f);
	}
}


void USMGA_Hold::ClientRPCSendHoldResult_Implementation(bool bNewSuccessHold)
{
	bSuccessHold = bNewSuccessHold;
}
