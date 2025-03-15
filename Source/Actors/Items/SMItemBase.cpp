// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Data/DataAsset/SMItemDataAsset.h"
#include "Utilities/SMCollision.h"

ASMItemBase::ASMItemBase(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootComponent);
	ColliderComponent->InitSphereRadius(50.0f);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(ColliderComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(ColliderComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMItemBase::ActivateItem(AActor* InActivator)
{
	Activator = InActivator;
	(void)OnUsedItem.ExecuteIfBound();
}

void ASMItemBase::MulticastPlayNiagara_Implementation(UNiagaraSystem* NiagaraSystem, FVector SpawnLocation)
{
	if (NiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, SpawnLocation, FRotator::ZeroRotator, FVector::OneVector, false, true, ENCPoolMethod::AutoRelease);
	}
}

void ASMItemBase::DeactivateItem()
{
	Destroy();
}

const FGameplayTagContainer& ASMItemBase::GetItemTags() const
{
	ensure(DataAsset);
	return DataAsset->ItemTags;
}
