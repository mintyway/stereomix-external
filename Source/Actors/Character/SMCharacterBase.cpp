// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterBase.h"

#include "Engine/AssetManager.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Net/UnrealNetwork.h"


ASMCharacterBase::ASMCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	// {
	// 	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(TEXT("Character"), TEXT("DA_ElectricGuitarCharacter"));
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("로드요청: %s"), *AssetManager->GetName());
	// 	AssetManager->LoadPrimaryAsset(PrimaryAssetId, TArray<FName>(), FStreamableDelegate::CreateWeakLambda(this, [this](FPrimaryAssetId AssetId) {
	// 		if (const UAssetManager* AM = UAssetManager::GetIfInitialized())
	// 		{
	// 			if (const UObject* DataAsset = AM->GetPrimaryAssetObject(AssetId))
	// 			{
	// 				UE_LOG(LogTemp, Warning, TEXT("비동기 로드완료: %s"), *DataAsset->GetName());
	// 			}
	// 		}
	// 	}, PrimaryAssetId));
	//
	// 	if (const FSoftObjectPath AssetPath = AssetManager->GetPrimaryAssetPath(PrimaryAssetId); AssetPath.IsValid())
	// 	{
	// 		if (const UObject* LoadedData = AssetManager->GetStreamableManager().LoadSynchronous(AssetPath))
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("동기 로드완료: %s"), *LoadedData->GetName());
	// 		}
	// 	}
	//
	// 	if (const UObject* LoadedObject = AssetManager->GetPrimaryAssetObject(PrimaryAssetId))
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("로드 됨: %s"), *LoadedObject->GetName());
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("로드 안 됨"));
	// 	}
	// }
}

void ASMCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Weapon);
}

void ASMCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	OnPossessed.Broadcast();
}

void ASMCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	OnPossessed.Broadcast();
}

void ASMCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ASMCharacterBase::RecalculateDefaultStencil()
{
	DefaultShaderStencil = ESMShaderStencil::NonOutline;
}

void ASMCharacterBase::ApplyDefaultStencil()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (USkeletalMeshComponent* CachedMeshComponent = GetMesh())
	{
		CachedMeshComponent->SetCustomDepthStencilValue(static_cast<int32>(DefaultShaderStencil));
	}

	const ASMWeaponBase* CachedWeapon = GetWeapon();
	if (UMeshComponent* WeaponMesh = CachedWeapon ? CachedWeapon->GetWeaponMeshComponent() : nullptr)
	{
		WeaponMesh->SetCustomDepthStencilValue(static_cast<int32>(DefaultShaderStencil));
	}
}

void ASMCharacterBase::OnRep_Weapon()
{
	ApplyDefaultStencil();
}
