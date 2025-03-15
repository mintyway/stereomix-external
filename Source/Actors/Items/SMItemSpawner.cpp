// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemSpawner.h"

#include "SMItemBase.h"
#include "CADKernel/Math/SlopeUtils.h"
#include "Components/WidgetComponent.h"
#include "Games/Round/SMRoundComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Item/SMItemSpawnTimerWidget.h"
#include "Utilities/SMCollision.h"


ASMItemSpawner::ASMItemSpawner()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	ItemSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSocketComponent"));
	ItemSocketComponent->SetupAttachment(MeshComponent);

	ItemSpawnTimerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemSpawnTimerWidgetComponent"));
	ItemSpawnTimerWidgetComponent->SetupAttachment(SceneComponent);
}

void ASMItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
		{
			RoundComponent->OnRoundStateUpdatedEvent.AddUObject(this, &ThisClass::OnRoundStateUpdated);
		}
	}

	if (USMItemSpawnTimerWidget* ItemSpawnTimerWidget = Cast<USMItemSpawnTimerWidget>(ItemSpawnTimerWidgetComponent->GetWidget()))
	{
		ItemSpawnTimerWidget->SetItemSpawner(this);
		RemainingCooldownSeconds = FMath::Max(RemainingCooldownSeconds, 0.0f);
		const bool bIsNearlyZero = FMath::IsNearlyZero(RemainingCooldownSeconds, UE::CADKernel::Slope::Epsilon);
		const float RemainingCooldownPercent = FMath::Clamp(bIsNearlyZero ? 1.0f : 1.0f - (RemainingCooldownSeconds / SpawnCooldown), 0.0f, 1.0f);
		OnRemainingCooldownChanged.Broadcast(RemainingCooldownSeconds, RemainingCooldownPercent);
	}
}

void ASMItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCooldown(DeltaTime);
}

void ASMItemSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMItemSpawner, RemainingCooldownSeconds);
}

void ASMItemSpawner::SpawnTimerStart()
{
	ResetSpawnerState();

	RemainingCooldownSeconds = SpawnCooldown;

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::SpawnTimerCallback, SpawnCooldown, false);
}

void ASMItemSpawner::SpawnTimerCallback()
{
	SpawnItem();
}

void ASMItemSpawner::SpawnItem()
{
	Item = GetWorld()->SpawnActor<ASMItemBase>(SpawnItemClass, ItemSocketComponent->GetComponentTransform());
	if (ensureAlways(Item))
	{
		// 아이템이 사용되거나 파괴되면 다시 타이머를 시작합니다.
		Item->OnUsedItem.BindUObject(this, &ThisClass::OnUsedItem);
		Item->OnDestroyed.AddDynamic(this, &ThisClass::OnDestroyedItems);
	}
}

void ASMItemSpawner::OnUsedItem()
{
	SpawnTimerStart();
}

void ASMItemSpawner::OnDestroyedItems(AActor* DestroyedActor)
{
	SpawnTimerStart();
}

void ASMItemSpawner::ResetSpawnerState()
{
	if (Item)
	{
		Item->OnUsedItem.Unbind();
		Item->OnDestroyed.RemoveAll(this);
	}

	Item = nullptr;
}

void ASMItemSpawner::UpdateCooldown(float DeltaTime)
{
	if (!FMath::IsNearlyZero(RemainingCooldownSeconds, UE::CADKernel::Slope::Epsilon))
	{
		RemainingCooldownSeconds = FMath::Max(RemainingCooldownSeconds - DeltaTime, 0.0f);
		const bool bIsNearlyZero = FMath::IsNearlyZero(RemainingCooldownSeconds, UE::CADKernel::Slope::Epsilon);
		const float RemainingCooldownPercent = FMath::Clamp(bIsNearlyZero ? 1.0f : 1.0f - (RemainingCooldownSeconds / SpawnCooldown), 0.0f, 1.0f);
		OnRemainingCooldownChanged.Broadcast(RemainingCooldownSeconds, RemainingCooldownPercent);
	}
}

void ASMItemSpawner::OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState)
{
	if (NewRoundState >= SpawnRound)
	{
		if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
		{
			RoundComponent->OnRoundStateUpdatedEvent.RemoveAll(this);
		}

		if (bUseImmediatelySpawn)
		{
			SpawnItem();
		}
		else
		{
			SpawnTimerStart();
		}
	}
}
