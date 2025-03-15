// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_BassNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimationAsset.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_BassNoiseBreakAdditionalEffects.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Actors/Tiles/SMTile.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/DataAsset/Character/SMBassCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMAnimationFunctionLibrary.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMCollision.h"

USMGA_BassNoiseBreak::USMGA_BassNoiseBreak()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags.AddTag(SMTags::Ability::NoiseBreak_Bass);

	if (const FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(ESMCharacterType::Bass))
	{
		Damage = NoiseBreakData->Damage;
		MaxDistanceByTile = NoiseBreakData->DistanceByTile;
		CaptureSize = NoiseBreakData->CaptureSize;
	}
}

void USMGA_BassNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const UWorld* World = GetWorld();
	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	USMAbilitySystemComponent* OwnerASC = GetASC();
	const USMHIC_Character* OwnerHIC = GetHIC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!World || !OwnerCharacter || !OwnerASC || !OwnerHIC || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// 노이즈 브레이크에 필요한 상태로 변경합니다.
	if (UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent())
	{
		OriginalCollisionProfileName = OwnerCapsule->GetCollisionProfileName();
		OwnerCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);
	}
	if (UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement())
	{
		OwnerMovement->SetMovementMode(MOVE_Flying);
	}

	// 몽타주를 재생합니다.
	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();
	if (CachedNoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[OwnerTeam]; CachedNoiseBreakMontage)
	{
		OwnerASC->PlayMontage(this, ActivationInfo, CachedNoiseBreakMontage, 1.0f);
	}

	// 노이즈 브레이크는 애님노티파이에 의해 종료됩니다. 노이즈 브레이크 애니메이션이 끝나기전에 움직일 수 있도록 하기 위함입니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Common::NoiseBreakEnd);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnNoiseBreakEndedCallback);
	WaitGameplayEventTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		// 베이스 노이즈 브레이크에 필요한 추가 이펙트 재생을 담당하는 태스크를 실행합니다.
		USMAT_BassNoiseBreakAdditionalEffects* AdditionalEffectsTask = USMAT_BassNoiseBreakAdditionalEffects::BassNoiseBreakAdditionalEffects(this);
		AdditionalEffectsTask->ReadyForActivation();

		// 도약 전 딜레이만큼 지연시킨뒤 도약합니다.
		FTimerHandle PreLeapTimerHandle;
		const float MaxDistance = MaxDistanceByTile * USMTileFunctionLibrary::DefaultTileSize;
		OwnerCharacter->GetTileLocationFromCursor(NoiseBreakTargetLocation, MaxDistance);
		const float PreLeapDuration = USMAnimationFunctionLibrary::GetSectionLengthWithRate(CachedNoiseBreakMontage, TEXT("Default"));
		World->GetTimerManager().SetTimer(PreLeapTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnPreLeapTimerCallback, NoiseBreakTargetLocation), PreLeapDuration, false);

		const FGameplayCueParameters GCParams = MakeGameplayCueParametersAttached(OwnerCharacter, FVector::ZeroVector, FVector::ZeroVector);
		OwnerASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Bass::NoiseBreak, GCParams);
	}

	// 노이즈 브레이크 시작을 타겟에게 알립니다.
	if (K2_HasAuthority())
	{
		AActor* TargetActor = OwnerHIC->GetActorIAmHolding();
		if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
		{
			TargetHIC->OnNoiseBreakStarted(OwnerCharacter);
		}
	}
}

void USMGA_BassNoiseBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (const ASMPlayerCharacterBase* OwnerCharacter = GetCharacter())
	{
		if (UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent())
		{
			OwnerCapsule->SetCollisionProfileName(OriginalCollisionProfileName);
		}

		if (UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement())
		{
			OwnerMovement->SetMovementMode(MOVE_Walking);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_BassNoiseBreak::OnPreLeapTimerCallback(FVector EndLocation)
{
	LeapCharacter(EndLocation);

	if (!K2_HasAuthority()) // Authority를 갖지 않는 경우 서버에도 전파합니다.
	{
		ServerSendNoiseBreakData(EndLocation);
	}
}

void USMGA_BassNoiseBreak::LeapCharacter(const FVector& TargetLocation)
{
	const UWorld* World = GetWorld();
	const ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	const USMBassCharacterDataAsset* OwnerDataAsset = GetDataAsset<USMBassCharacterDataAsset>();
	UCharacterMovementComponent* OwnerMovement = OwnerCharacter ? OwnerCharacter->GetCharacterMovement() : nullptr;
	if (!World || !OwnerDataAsset || !OwnerMovement)
	{
		EndAbilityByCancel();
		return;
	}

	const float LeapDuration = USMAnimationFunctionLibrary::GetSectionLengthWithRate(CachedNoiseBreakMontage, TEXT("Leap"));

	const TSharedPtr<FRootMotionSource_ConstantForce> RMSForXAxis = MakeShared<FRootMotionSource_ConstantForce>();
	RMSForXAxis->AccumulateMode = ERootMotionAccumulateMode::Override;
	RMSForXAxis->Duration = LeapDuration;
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector OwnerToTarget = TargetLocation - OwnerLocation;
	const float Distance = OwnerToTarget.Size2D();
	const float Force = Distance / LeapDuration;
	RMSForXAxis->Force = OwnerToTarget.GetSafeNormal2D() * Force;
	RMSForXAxis->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	RMSForXAxis->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
	OwnerMovement->ApplyRootMotionSource(RMSForXAxis);

	TMap<ESMTeam, TObjectPtr<UCurveFloat>> NoiseBreakRootMotionCurveMap = OwnerDataAsset->NoiseBreakRootMotionCurve;
	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();
	if (UCurveFloat* NoiseBreakRootMotionCurve = NoiseBreakRootMotionCurveMap.Contains(OwnerTeam) ? NoiseBreakRootMotionCurveMap[OwnerTeam] : nullptr)
	{
		const TSharedPtr<FRootMotionSource_ConstantForce> RMSForZAxis = MakeShared<FRootMotionSource_ConstantForce>();
		RMSForZAxis->AccumulateMode = ERootMotionAccumulateMode::Additive;
		RMSForZAxis->Duration = LeapDuration;
		RMSForZAxis->Force = FVector::UpVector;
		RMSForZAxis->StrengthOverTime = NoiseBreakRootMotionCurve;
		RMSForZAxis->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
		RMSForZAxis->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
		OwnerMovement->ApplyRootMotionSource(RMSForZAxis);
	}

	FTimerHandle LeapEndTimerHandle;
	World->GetTimerManager().SetTimer(LeapEndTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnLeapEnded), LeapDuration, false);
}

void USMGA_BassNoiseBreak::ServerSendNoiseBreakData_Implementation(const FVector_NetQuantize10& TargetLocation)
{
	NoiseBreakTargetLocation = TargetLocation;
	LeapCharacter(TargetLocation);
}

void USMGA_BassNoiseBreak::OnLeapEnded()
{
	ASMBassCharacter* OwnerCharacter = GetCharacter<ASMBassCharacter>();
	const USMAbilitySystemComponent* OwnerASC = GetASC();
	USMHIC_Character* OwnerHIC = GetHIC();
	if (!OwnerCharacter || !OwnerASC || !OwnerHIC)
	{
		EndAbilityByCancel();
		return;
	}

	if (K2_HasAuthority())
	{
		AActor* TargetActor = OwnerHIC->GetActorIAmHolding();
		TArray<ASMTile*> CapturedTiles;
		if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
		{
			const TArray<ASMTile*> TilesToBeCaptured = GetTilesToBeCaptured();
			const TSharedRef<FSMNoiseBreakData> NoiseBreakData = MakeShared<FSMNoiseBreakData>();
			NoiseBreakData->TilesToBeTriggered = USMTileFunctionLibrary::ConvertToWeakPtrArray(TilesToBeCaptured);
			TargetHIC->OnNoiseBreakApplied(OwnerCharacter, NoiseBreakData);

			if (TargetHIC->ShouldCaptureTilesFromNoiseBreak())
			{
				CapturedTiles = USMTileFunctionLibrary::CaptureTiles(GetWorld(), TilesToBeCaptured, OwnerCharacter);
			}

			if (TargetHIC->ShouldApplyDamageFromNoiseBreak())
			{
				PerformBurstAttack(OwnerCharacter->GetActorLocation(), SMTags::GameplayCue::Bass::NoiseBreakBurstHit);
			}
		}

		const APawn* TargetPawn = Cast<APawn>(TargetActor);
		APlayerController* TargetPlayerController = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
		const USMPlayerCharacterDataAsset* SourceDataAsset = OwnerCharacter ? OwnerCharacter->GetDataAsset() : nullptr;
		if (TargetPlayerController && SourceDataAsset)
		{
			TargetPlayerController->ClientStartCameraShake(SourceDataAsset->NoiseBreakCameraShake);
		}

		OwnerHIC->SetActorIAmHolding(nullptr);
		OnNoiseBreakSucceed.Broadcast(CapturedTiles);
	}

	if (IsLocallyControlled())
	{
		APlayerController* OwnerController = OwnerCharacter->GetController<APlayerController>();
		const USMPlayerCharacterDataAsset* OwnerDataAsset = GetDataAsset();
		if (OwnerController && OwnerDataAsset)
		{
			OwnerController->ClientStartCameraShake(OwnerDataAsset->NoiseBreakCameraShake);
		}

		const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
		const ASMTile* Tile = GetTileFromLocation(OwnerLocation);
		const FVector TileLocation = Tile ? Tile->GetTileLocation() : FVector::ZeroVector;
		const FVector Offset(0.0, 0.0, 10.0); // 타일과 겹치게 되면 현재 이펙트가 잘 안 보이는데 이를 방지하는 오프셋입니다.

		const AActor* TargetActor = OwnerHIC->GetActorIAmHolding();
		const float HoldingType = TargetActor ? (Cast<ASMCharacterBase>(TargetActor) ? 0.0f : 1.0f) : 0.0f; // 0이면 캐릭터, 1이면 아이템입니다.

		FGameplayCueParameters GCParams = MakeGameplayCueParameters(TileLocation + Offset, FVector::ZeroVector);
		GCParams.RawMagnitude = HoldingType;
		OwnerASC->ExecuteGC(OwnerCharacter, SMTags::GameplayCue::Bass::NoiseBreakBurst, GCParams);
	}
}

void USMGA_BassNoiseBreak::OnNoiseBreakEndedCallback(FGameplayEventData Payload)
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}

TArray<ASMTile*> USMGA_BassNoiseBreak::GetTilesToBeCaptured()
{
	const UWorld* World = GetWorld();
	const ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	if (!World || !OwnerCharacter)
	{
		return TArray<ASMTile*>();
	}

	const FVector SourceLocation = OwnerCharacter->GetActorLocation();
	constexpr float Offset = USMTileFunctionLibrary::DefaultTileSize / 4.0f;
	const float HalfSize = Offset + (USMTileFunctionLibrary::DefaultTileSize * (CaptureSize - 1));
	const FVector BoxExtend(HalfSize);

	return USMTileFunctionLibrary::GetTilesInBox(World, SourceLocation, BoxExtend);
}
