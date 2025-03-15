// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ImpactArrow.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Data/SMControlData.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Player/SMPlayerController.h"
#include "Utilities/SMCollision.h"


USMGA_ImpactArrow::USMGA_ImpactArrow()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	AbilityTags.AddTag(SMTags::Ability::Skill_Piano);
	ActivationOwnedTags.AddTag(SMTags::Character::State::Piano::ImpactArrow);

	if (const FSMCharacterSkillData* SkillData = USMDataTableFunctionLibrary::GetCharacterSkillData(ESMCharacterType::Piano))
	{
		Damage = SkillData->Damage;
		MaxDistanceByTile = SkillData->DistanceByTile;
		ImpactPreDelay = SkillData->StartUpTime;
		Radius = SkillData->Range;
		KnockbackMagnitude = SkillData->Magnitude;
	}
}

void USMGA_ImpactArrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	const USMAbilitySystemComponent* OwnerASC = GetASC();
	const USMPlayerCharacterDataAsset* OwnerDataAsset = GetDataAsset();
	if (!OwnerCharacter || !OwnerASC || !OwnerDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	const FName TaskName = TEXT("MontageTask");
	const ESMTeam SourceTeam = OwnerCharacter->GetTeam();
	UAnimMontage* Montage = OwnerDataAsset->SkillMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		ASMPlayerController* OwnerPlayerController = OwnerCharacter ? OwnerCharacter->GetController<ASMPlayerController>() : nullptr;
		if (const USMControlData* ControlData = OwnerPlayerController ? OwnerCharacter->GetDataAsset()->InputActions : nullptr)
		{
			ShootInputComponent = NewObject<UEnhancedInputComponent>(OwnerPlayerController);
			ShootInputComponent->RegisterComponent();
			ShootInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Started, this, &ThisClass::OnShoot);
			OwnerPlayerController->PushInputComponent(ShootInputComponent);
		}

		const float MaxDistance = MaxDistanceByTile * 150.0f;

		const TMap<ESMTeam, TObjectPtr<UNiagaraSystem>>& CachedVFX = OwnerDataAsset->SkillIndicatorVFX;
		const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();
		UNiagaraSystem* ImpactArrowIndicatorVFX = CachedVFX.Contains(OwnerTeam) ? CachedVFX[OwnerTeam] : nullptr;

		SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, ImpactArrowIndicatorVFX, USMAT_SkillIndicator::EIndicatorAlignment::Ground, MaxDistance);
		SkillIndicatorTask->ReadyForActivation();

		FGameplayCueParameters GCParams = MakeGameplayCueParameters();
		OwnerASC->AddGC(OwnerCharacter, SMTags::GameplayCue::Piano::ImpactArrowReady, GCParams);
	}
}

void USMGA_ImpactArrow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();

	if (ShootInputComponent)
	{
		if (ASMPlayerController* OwnerPlayerController = OwnerCharacter ? OwnerCharacter->GetController<ASMPlayerController>() : nullptr)
		{
			OwnerPlayerController->PopInputComponent(ShootInputComponent);
			ShootInputComponent = nullptr;
		}
	}

	if (SkillIndicatorTask)
	{
		SkillIndicatorTask->EndTask();
		SkillIndicatorTask = nullptr;
	}

	if (const USMAbilitySystemComponent* OwnerASC = GetASC())
	{
		FGameplayCueParameters GCParams;
		GCParams.SourceObject = OwnerCharacter;
		GCParams.RawMagnitude = 0.0f;
		OwnerASC->RemoveGC(OwnerCharacter, SMTags::GameplayCue::Piano::ImpactArrowReady, GCParams);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_ImpactArrow::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const FName SectionName = TEXT("Cancel");
	MontageJumpToSection(SectionName);

	if (const USMAbilitySystemComponent* OwnerASC = GetASC())
	{
		AActor* OwnerActor = GetAvatarActor();
		FGameplayCueParameters GCParams = MakeGameplayCueParameters();
		GCParams.RawMagnitude = 0.0f;
		OwnerASC->RemoveGC(OwnerActor, SMTags::GameplayCue::Piano::ImpactArrowReady, GCParams);
	}

	K2_EndAbility();
}

void USMGA_ImpactArrow::OnShoot()
{
	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	const USMAbilitySystemComponent* OwnerASC = GetASC();
	if (!OwnerCharacter || !OwnerASC)
	{
		return;
	}

	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	FGameplayCueParameters ReadyEndGCParams;
	ReadyEndGCParams.SourceObject = OwnerCharacter;
	ReadyEndGCParams.RawMagnitude = 1.0f;
	OwnerASC->RemoveGC(OwnerCharacter, SMTags::GameplayCue::Piano::ImpactArrowReady, ReadyEndGCParams);

	const FVector Direction = FRotator(60.0, 0.0, 0.0).Vector();
	const FVector OffsetLocation = Direction * 300.0;
	FGameplayCueParameters ShootGCParams = MakeGameplayCueParametersAttached(OwnerCharacter, OffsetLocation, Direction);
	OwnerASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Piano::ImpactArrow, ShootGCParams);

	const float MaxDistance = MaxDistanceByTile * 150.0f;
	const FVector ImpactLocation = OwnerCharacter->GetLocationFromCursor(true, MaxDistance);
	ServerRequestShoot(ImpactLocation);


	K2_EndAbilityLocally(); // 클라이언트에서만 미리 종료해 인디케이터와 입력컴포넌트를 제거합니다. 
}

void USMGA_ImpactArrow::ServerRequestShoot_Implementation(const FVector_NetQuantize10& ImpactLocation)
{
	if (const UWorld* World = GetWorld())
	{
		// EndAbility가 호출되어도 타이머가 초기화 되지 않도록 일반 람다식을 사용합니다. 이렇게하면 this를 바인드하는 것이 아니라 타이머 초기화 대상에서 벗어날 수 있습니다.
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr = MakeWeakObjectPtr(this), ImpactLocation] {
			if (ThisWeakPtr.IsValid())
			{
				ThisWeakPtr->OnImpact(ImpactLocation);
			}
		}, ImpactPreDelay, false);
	}

	K2_CommitAbilityCost();

	K2_EndAbility();
}

void USMGA_ImpactArrow::OnImpact(const FVector& ImpactLocation)
{
	if (!K2_HasAuthority())
	{
		return;
	}

	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	const USMAbilitySystemComponent* OwnerASC = GetASC();
	if (!OwnerCharacter || !OwnerASC)
	{
		return;
	}

	const USMPlayerCharacterDataAsset* OwnerDataAsset = GetDataAsset();
	const TSubclassOf<UCameraShakeBase>& SkillHitCameraShakeClass =
		OwnerDataAsset
			? OwnerDataAsset->SkillHitCameraShake
			: nullptr;

	for (AActor* HitActor : GetImpactHitActors(ImpactLocation))
	{
		if (!TryGiveDamageToTarget(HitActor, Damage))
		{
			continue;
		}

		if (ASMCharacterBase* HitCharacter = Cast<ASMCharacterBase>(HitActor))
		{
			const FVector ImpactLocationToHitCharacterDirection = (HitCharacter->GetActorLocation() - ImpactLocation).GetSafeNormal2D();
			const FRotator PushBackRotation = ImpactLocationToHitCharacterDirection.Rotation() + FRotator(15.0, 0.0, 0.0);
			const FVector PushBackVelocity = PushBackRotation.Vector() * KnockbackMagnitude;

			if (ASMPlayerCharacterBase* PlayerCharacter = Cast<ASMPlayerCharacterBase>(HitCharacter))
			{
				PlayerCharacter->ClientRPCCharacterPushBack(PushBackVelocity);
			}
			else
			{
				HitCharacter->GetCharacterMovement()->AddImpulse(PushBackVelocity);
			}

			if (APlayerController* PlayerController = OwnerCharacter->GetController<APlayerController>())
			{
				PlayerController->ClientStartCameraShake(SkillHitCameraShakeClass);
			}

			if (APlayerController* TargetPlayerController = HitCharacter->GetController<APlayerController>())
			{
				TargetPlayerController->ClientStartCameraShake(SkillHitCameraShakeClass);
			}

			// 방향에 맞게 이펙트를 재생합니다.
			FGameplayCueParameters GCParams = MakeGameplayCueParametersAttached(OwnerCharacter, FVector::ZeroVector, ImpactLocationToHitCharacterDirection);
			OwnerASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Piano::ImpactArrowHit, GCParams);

			OnSkillHit.Broadcast();
		}
	}
	FGameplayCueParameters GCParams = MakeGameplayCueParameters(ImpactLocation, FVector::ZeroVector);
	OwnerASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Piano::ImpactArrowExplosion, GCParams);

	if (bUseDebug)
	{
		DrawDebugSphere(GetWorld(), ImpactLocation, Radius, 16, FColor::Red, false, 1.0f);
	}
}

TArray<AActor*> USMGA_ImpactArrow::GetImpactHitActors(const FVector& ImpactLocation)
{
	TArray<AActor*> Result;

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, ImpactLocation, FQuat::Identity, SMCollisionTraceChannel::ActionTC, FCollisionShape::MakeSphere(Radius)))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			if (AActor* TargetActor = OverlapResult.GetActor())
			{
				Result.Add(TargetActor);
			}
		}
	}

	return Result;
}
