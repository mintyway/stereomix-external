// Copyright Studio Surround. All Rights Reserved.


#include "SMFragileObstacle.h"

#include "FMODBlueprintStatics.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Audio/SMFMODBlueprintStatics.h"
#include "CADKernel/Math/SlopeUtils.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ObjectSaveContext.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMFragileObstacle::ASMFragileObstacle()
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::Obstacle);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(MeshComponent);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->SetMobility(EComponentMobility::Static);

	DurabilityThresholds.Add({ 0.0f, 0.0f, 0.0f, nullptr });
}

void ASMFragileObstacle::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	CurrentDurability = Durability;
}

void ASMFragileObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMFragileObstacle, CurrentDurability);
	DOREPLIFETIME(ASMFragileObstacle, Durability);
	DOREPLIFETIME(ASMFragileObstacle, LastAttacker);
}

void ASMFragileObstacle::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	DurabilityThresholds.Sort([](const FSMFragileObstacleDurabilityThresholdData& lhs, const FSMFragileObstacleDurabilityThresholdData& rhs) {
		return lhs.DurabilityRatio > rhs.DurabilityRatio;
	});
}

void ASMFragileObstacle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetCollisionEnabled(true);

	if (!bSpawnImmediately)
	{
		SetCollisionEnabled(false);
	}

	DurabilityThresholds.Sort([](const FSMFragileObstacleDurabilityThresholdData& lhs, const FSMFragileObstacleDurabilityThresholdData& rhs) {
		return lhs.DurabilityRatio > rhs.DurabilityRatio;
	});
}

void ASMFragileObstacle::ServerSetCurrentDurability_Implementation(float NewCurrentDurability)
{
	CurrentDurability = NewCurrentDurability;

	OnRep_CurrentDurability();
}

void ASMFragileObstacle::ServerSetMaxDurability_Implementation(float NewMaxDurability)
{
	Durability = NewMaxDurability;

	OnRep_Durability();
}

void ASMFragileObstacle::ServerRestoreObstacle_Implementation()
{
	ServerSetCurrentDurability(Durability);
}

void ASMFragileObstacle::SetCollisionEnabled(bool bNewIsCollisionEnabled)
{
	const FName CollisionProfileName = bNewIsCollisionEnabled ? SMCollisionProfileName::Obstacle : SMCollisionProfileName::NoCollision;
	if (ColliderComponent->GetCollisionProfileName() != CollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(CollisionProfileName);
	}
}

void ASMFragileObstacle::UnloadObstacle()
{
	if (CurrentDurability < 0.0f)
	{
		return;
	}

	Super::UnloadObstacle();
}

void ASMFragileObstacle::ReceiveDamage(AActor* NewAttacker, float InDamageAmount, const FGameplayTagContainer& AggregatedSourceTags)
{
	LastAttacker = NewAttacker;
	ServerSetCurrentDurability(FMath::Clamp(CurrentDurability - InDamageAmount, 0.0f, Durability));
	NET_LOG(this, Log, TEXT("%s Damage: %f, CurrentHealth: %f"), *GetName(), InDamageAmount, CurrentDurability);
}

void ASMFragileObstacle::ClientPlayCrackSound_Implementation(float Step)
{
	if (CrackSound)
	{
		const FFMODEventInstance HitSoundInstance = USMFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), CrackSound, GetActorTransform(), true);
		USMFMODBlueprintStatics::EventInstanceSetParameter(HitSoundInstance, TEXT("ObjectMaterial"), ObjectMaterial);
		USMFMODBlueprintStatics::EventInstanceSetParameter(HitSoundInstance, TEXT("ObjectCrackStep"), Step);
	}
}

void ASMFragileObstacle::OnRep_CurrentDurability()
{
	UpdateVisualBasedOnDurability();

	SetCollisionEnabled(true);
	if (CurrentDurability <= 0.0f)
	{
		SetCollisionEnabled(false);
	}
}

void ASMFragileObstacle::OnRep_Durability()
{
	UpdateVisualBasedOnDurability();
}

void ASMFragileObstacle::UpdateVisualBasedOnDurability()
{
	UNiagaraSystem* NewNiagaraSystem = nullptr;
	UAnimMontage* NewAnimMontage = DefaultHitAnimMontage;

	const float CurrentDurabilityRatio = CurrentDurability / Durability;
	float DurabilityRatioLevel = 1.0f;
	float CurrentStep = 0.0f;

	for (const auto& [DurabilityRatio, DamageParam, CrackStep, NiagaraSystem, SkeletalMesh, AnimMontage] : DurabilityThresholds)
	{
		if (FMath::IsNearlyEqual(CurrentDurabilityRatio, DurabilityRatio, UE::CADKernel::Slope::Epsilon) ||
		    CurrentDurabilityRatio <= DurabilityRatio)
		{
			DurabilityRatioLevel = DurabilityRatio;

			NewNiagaraSystem = NiagaraSystem;
			NewAnimMontage = AnimMontage;
			CurrentStep = CrackStep;

			SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
			if (SkeletalMesh)
			{
				SkeletalMeshComponent->SetScalarParameterValueOnMaterials("Damage", DamageParam);
			}
		}
	}

	if (SkeletalMeshComponent->GetSkeletalMeshAsset() && NewAnimMontage)
	{
		SkeletalMeshComponent->PlayAnimation(NewAnimMontage, false);
	}

	// 단계 변화 이펙트 처리 시 히트 이펙트 처리 안하도록 처리
	if (FMath::IsNearlyEqual(DurabilityRatioLevel, LatestDurabilityRatioLevel, UE::CADKernel::Slope::Epsilon) && HitEffect)
	{
		ClientPlayHitSound(CurrentStep);
		if (UNiagaraComponent* InstanceHitNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, OriginalNiagaraSystemLocation, OriginalNiagaraSystemRotation, FVector::OneVector, false, true, ENCPoolMethod::AutoRelease))
		{
			InstanceHitNiagaraComponent->SetVariableFloat("X", EffectParameterX);
			InstanceHitNiagaraComponent->SetVariableFloat("Y", EffectParameterY);
			InstanceHitNiagaraComponent->SetVariableFloat("Z", EffectParameterZ);
		}
	}
	else
	{
		LatestDurabilityRatioLevel = DurabilityRatioLevel;

		ClientPlayCrackSound(CurrentStep);

		if (NiagaraComponent && NiagaraComponent->IsValidLowLevelFast())
		{
			if (NiagaraComponent->IsActive())
			{
				NiagaraComponent->DeactivateImmediate();
			}

			if (NiagaraComponent->PoolingMethod == ENCPoolMethod::ManualRelease)
			{
				NiagaraComponent->ReleaseToPool();
			}
		}

		if (NewNiagaraSystem)
		{
			NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NewNiagaraSystem, OriginalNiagaraSystemLocation, OriginalNiagaraSystemRotation, FVector::OneVector, false, true, ENCPoolMethod::AutoRelease);
			if (NiagaraComponent)
			{
				NiagaraComponent->SetVariableFloat("X", EffectParameterX);
				NiagaraComponent->SetVariableFloat("Y", EffectParameterY);
				NiagaraComponent->SetVariableFloat("Z", EffectParameterZ);
			}
		}
	}
}
