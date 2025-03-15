// Copyright Studio Surround. All Rights Reserved.


#include "SMObstacleBase.h"

#include "Engine/OverlapResult.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Audio/SMFMODBlueprintStatics.h"
#include "Components/BoxComponent.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMCollision.h"


ASMObstacleBase::ASMObstacleBase()
{
	bReplicates = true;

	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderComponent"));
	RootComponent = ColliderComponent;
	ColliderComponent->SetBoxExtent(FVector(75.0f, 75.0f, 75.0f));
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::StaticObstacle);
	ColliderComponent->SetMobility(EComponentMobility::Static);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->SetMobility(EComponentMobility::Static);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	NiagaraComponent->SetMobility(EComponentMobility::Static);
}

void ASMObstacleBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginalMesh = MeshComponent->GetStaticMesh();
	OriginalNiagaraSystem = NiagaraComponent->GetAsset();
	OriginalNiagaraSystemLocation = NiagaraComponent->GetComponentLocation();
	OriginalNiagaraSystemRotation = NiagaraComponent->GetComponentRotation();

	SetCollisionEnabled(false);

	if (bSpawnImmediately)
	{
		SetCollisionEnabled(true);
	}
	else
	{
		MeshComponent->SetStaticMesh(nullptr);
		NiagaraComponent->DeactivateImmediate();
	}
}

void ASMObstacleBase::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();

	if (!HasAuthority() || !World || bSpawnImmediately)
	{
		return;
	}

	TWeakObjectPtr<ASMObstacleBase> ThisWeakPtr(this);

	float AccumulatedTime = 0.0f;

	auto SetPreSpawn = [ThisWeakPtr](UStaticMesh* NewStaticMesh, UNiagaraSystem* NewNiagaraSystem, ENCPoolMethod PoolMethod = ENCPoolMethod::ManualRelease) {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->MulticastSetCollisionPreset(SMCollisionProfileName::PassThroughObstacle);
			ThisWeakPtr->MulticastSetMeshAndNiagaraSystem(NewStaticMesh, NewNiagaraSystem, PoolMethod);
		}
	};

	auto SetSpawn = [ThisWeakPtr](UStaticMesh* NewStaticMesh, UNiagaraSystem* NewNiagaraSystem, ENCPoolMethod PoolMethod = ENCPoolMethod::ManualRelease) {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->MulticastSetCollisionEnabled(true);
			ThisWeakPtr->MulticastSetMeshAndNiagaraSystem(NewStaticMesh, NewNiagaraSystem, PoolMethod);
			ThisWeakPtr->ServerTeleportToSafeLocation();
		}
	};

	AccumulatedTime += SpawnDelay;

	FTimerHandle PreSpawnEffectTimerHandle;
	UNiagaraSystem* CachedPreSpawnEffect = PreSpawnEffect;
	AccumulatedTime += PreSpawnEffectDuration;
	World->GetTimerManager().SetTimer(PreSpawnEffectTimerHandle, [SetPreSpawn, CachedPreSpawnEffect] {
		SetPreSpawn(nullptr, CachedPreSpawnEffect, ENCPoolMethod::AutoRelease);
	}, AccumulatedTime, false);

	FTimerHandle SpawnTimerHandle;
	UStaticMesh* CachedOriginalMesh = OriginalMesh;
	UNiagaraSystem* CachedOriginNiagaraSystem = OriginalNiagaraSystem;
	AccumulatedTime += SpawnEffectDuration;
	World->GetTimerManager().SetTimer(SpawnTimerHandle, [SetSpawn, CachedOriginalMesh, CachedOriginNiagaraSystem] {
		SetSpawn(CachedOriginalMesh, CachedOriginNiagaraSystem);
	}, AccumulatedTime, false);

	FTimerHandle AfterSpawnTimerHandle;
	AccumulatedTime += SpawnEffectDuration;
	World->GetTimerManager().SetTimer(AfterSpawnTimerHandle, [ThisWeakPtr] {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->ServerTeleportToSafeLocation();
		}
	}, AccumulatedTime, false);

	FTimerHandle FinalSpawnTimerHandle;
	AccumulatedTime += 1.0f;
	World->GetTimerManager().SetTimer(FinalSpawnTimerHandle, [ThisWeakPtr] {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->ServerTeleportToSafeLocation();
		}
	}, AccumulatedTime, false);
}

void ASMObstacleBase::SetCollisionEnabled(bool bNewIsCollisionEnabled)
{
	const FName CollisionProfileName = bNewIsCollisionEnabled ? SMCollisionProfileName::StaticObstacle : SMCollisionProfileName::NoCollision;
	if (ColliderComponent->GetCollisionProfileName() != CollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(CollisionProfileName);
	}
}

void ASMObstacleBase::UnloadObstacle()
{
	MulticastSetCollisionEnabled(false);
	MulticastSetMeshAndNiagaraSystem(nullptr, DestroyEffect, ENCPoolMethod::AutoRelease);
}

void ASMObstacleBase::MulticastSetCollisionPreset_Implementation(FName NewCollisionProfileName)
{
	if (ColliderComponent->GetCollisionProfileName() != NewCollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(NewCollisionProfileName);
	}
}

void ASMObstacleBase::ClientPlayHitSound_Implementation(float Step)
{
	if (HitSound)
	{
		const FFMODEventInstance HitSoundInstance = USMFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), HitSound, GetActorTransform(), true);
		USMFMODBlueprintStatics::EventInstanceSetParameter(HitSoundInstance, TEXT("ObjectMaterial"), ObjectMaterial);
		USMFMODBlueprintStatics::EventInstanceSetParameter(HitSoundInstance, TEXT("ObjectCrackStep"), Step);
	}
}

void ASMObstacleBase::ServerTeleportToSafeLocation_Implementation()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector ScaledBoxExtent = ColliderComponent->GetScaledBoxExtent();
	ScaledBoxExtent.Z = 500.0f;
	if (TArray<FOverlapResult> OverlapResults; World->OverlapMultiByChannel(OverlapResults, GetActorLocation(), FQuat::Identity, SMCollisionTraceChannel::ActionTC, FCollisionShape::MakeBox(ScaledBoxExtent)))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* TargetActor = OverlapResult.GetActor();
			if (!TargetActor)
			{
				continue;
			}

			if (ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor))
			{
				const FVector CurrentTargetActorLocation = TargetCharacter->GetActorLocation();
				constexpr float TileSize = USMTileFunctionLibrary::DefaultTileSize;
				const FVector BoxExtend = FVector(TileSize * 3, TileSize * 3, 500.0f);
				if (const ASMTile* ClosestTile = USMTileFunctionLibrary::FindClosestTileWithoutObstacles(World, CurrentTargetActorLocation, BoxExtend))
				{
					const FVector ClosestTileLocation = ClosestTile->GetTileLocation();
					TargetCharacter->SetActorLocation(FVector(ClosestTileLocation.X, ClosestTileLocation.Y, CurrentTargetActorLocation.Z));
				}
			}
		}
	}
}

void ASMObstacleBase::MulticastSetCollisionEnabled_Implementation(bool bNewIsCollisionEnabled)
{
	SetCollisionEnabled(bNewIsCollisionEnabled);
}

void ASMObstacleBase::MulticastSetMeshAndNiagaraSystem_Implementation(UStaticMesh* NewMesh, UNiagaraSystem* NewNiagaraSystem, ENCPoolMethod PoolMethod)
{
	if (!HasAuthority())
	{
		MeshComponent->SetStaticMesh(NewMesh);

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
			NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NewNiagaraSystem, OriginalNiagaraSystemLocation, OriginalNiagaraSystemRotation, FVector::OneVector, false, true, PoolMethod);
			if (NiagaraComponent)
			{
				NiagaraComponent->SetVariableFloat("X", EffectParameterX);
				NiagaraComponent->SetVariableFloat("Y", EffectParameterY);
				NiagaraComponent->SetVariableFloat("Z", EffectParameterZ);
			}
		}
	}
}
