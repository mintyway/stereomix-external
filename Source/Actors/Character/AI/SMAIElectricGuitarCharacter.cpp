﻿// Copyright Studio Surround. All Rights Reserved.


#include "SMAIElectricGuitarCharacter.h"

#include "GameplayTagContainer.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "Actors/Projectiles/Effect/SMEffectProjectileBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMProjectileFunctionLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"


ASMAIElectricGuitarCharacter::ASMAIElectricGuitarCharacter()
{
	if (const FSMCharacterAttackData* AttackData = USMDataTableFunctionLibrary::GetCharacterAttackData(ESMCharacterType::ElectricGuitar))
	{
		Damage = AttackData->Damage;
		MaxDistanceByTile = AttackData->DistanceByTile;
		ProjectileSpeed = AttackData->ProjectileSpeed;
		SpreadAngle = AttackData->SpreadAngle;
		AccuracyShootRate = AttackData->AccuracyAttackRate;
	}
}

void ASMAIElectricGuitarCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (USkeletalMeshComponent* CachedMesh = GetMesh())
	{
		CachedMesh->bRenderCustomDepth = true;
		CachedMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::RedOutline));
	}


	if (UMeshComponent* WeaponMesh = Weapon ? Weapon->GetWeaponMeshComponent() : nullptr)
	{
		WeaponMesh->bRenderCustomDepth = true;
		WeaponMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::RedOutline));
	}
}

void ASMAIElectricGuitarCharacter::Attack(AActor* AttackTarget)
{
	ASMEffectProjectileBase* Projectile = Cast<ASMEffectProjectileBase>(USMProjectileFunctionLibrary::GetElectricGuitarProjectile(GetWorld(), ESMTeam::FutureBass));
	if (!Projectile)
	{
		return;
	}

	const int RandomValue = FMath::Rand() % 100;
	float RandomYaw;
	if (RandomValue < AccuracyShootRate)
	{
		RandomYaw = (FMath::Rand() % 10) - 5.0f; // 10도 범위
	}
	else
	{
		RandomYaw = (FMath::Rand() % static_cast<int32>(SpreadAngle)) - (SpreadAngle / 2.0f);
	}

	const FVector LaunchDirection = ((AttackTarget->GetActorLocation() - GetActorLocation()).Rotation() + FRotator(0.0, RandomYaw, 0.0)).Vector();
	const float MaxDistance = MaxDistanceByTile * USMTileFunctionLibrary::DefaultTileSize;

	FSMProjectileParameters ProjectileParams;
	ProjectileParams.Owner = this;
	ProjectileParams.StartLocation = GetActorLocation();
	ProjectileParams.Normal = LaunchDirection;
	ProjectileParams.Speed = ProjectileSpeed;
	ProjectileParams.MaxDistance = MaxDistance;

	Projectile->OnProjectileHit.BindUObject(this, &ThisClass::OnProjectileHit);
	Projectile->Launch(ProjectileParams);

	OnAttack.Broadcast();
}

void ASMAIElectricGuitarCharacter::OnProjectileHit(AActor* HitActor)
{
	ISMDamageInterface* HitActorDamageInterface = Cast<ISMDamageInterface>(HitActor);
	if (HitActorDamageInterface && !HitActorDamageInterface->CanIgnoreAttack())
	{
		HitActorDamageInterface->ReceiveDamage(this, Damage, FGameplayTagContainer());
	}
}
