// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayPopSubsystem.h"

#include "SMGameplayPop.h"
#include "SMGameplayPopDescriptors.h"
#include "SMGameplayPopLog.h"
#include "SMGameplayPopSettings.h"
#include "Utilities/SMPool.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameplayPopSubsystem)

const FName DamagePopName = TEXT("Damage");
const FName ItemPopName = TEXT("Item");
const FName BuffPopName = TEXT("Buff");
const FName TilePopName = TEXT("Tile");

USMGameplayPopSubsystem* USMGameplayPopSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		return World->GetSubsystem<USMGameplayPopSubsystem>();
	}
	return nullptr;
}

bool USMGameplayPopSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	UWorld* World = Cast<UWorld>(Outer);
	if (!World->IsGameWorld())
	{
		return false;
	}
	if (World->GetNetMode() == NM_DedicatedServer)
	{
		return false;
	}
	return true;
}

bool USMGameplayPopSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void USMGameplayPopSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const USMGameplayPopSettings* Settings = GetDefault<USMGameplayPopSettings>();
	InitialPoolSize = Settings->InitialPoolSize;
	MaxActivatePopsPerTick = Settings->MaxActivatePopsPerTick;

	PopWidgetClassMap.Add(DamagePopName, Settings->DefaultDamagePopWidget);
	PopWidgetClassMap.Add(ItemPopName, Settings->DefaultItemPopWidget);
	PopWidgetClassMap.Add(BuffPopName, Settings->DefaultBuffPopWidget);
	PopWidgetClassMap.Add(TilePopName, Settings->DefaultTilePopWidget);

	InitializeActorPool();
	InitializeDescriptorPool();
}

void USMGameplayPopSubsystem::Deinitialize()
{
	Super::Deinitialize();
	for (auto& PoolPair : ActorPools)
	{
		USMActorPool* Pool = PoolPair.Value;
		Pool->Clear();
	}
	for (auto& PoolPair : DescriptorPools)
	{
		USMObjectPool* Pool = PoolPair.Value;
		Pool->Clear();
	}
}

TStatId USMGameplayPopSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USMGameplayPopSubsystem, STATGROUP_Tickables);
}

void USMGameplayPopSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < MaxActivatePopsPerTick; ++i)
	{
		if (!PendingPops.IsEmpty())
		{
			FSMPendingGameplayPop PendingPop = PendingPops.Pop();
			USMActorPool* ActorPool = ActorPools.FindChecked(PendingPop.PoolName);
			USMGameplayPopDescriptor* Descriptor = PendingPop.Descriptor;

			UClass* WidgetClass = PopWidgetClassMap.FindChecked(PendingPop.PoolName);

			ASMGameplayPop* NewPop = CastChecked<ASMGameplayPop>(ActorPool->GetOrCreate());
			if (!NewPop)
			{
				UE_LOG(LogStereoMixGameplayPop, Error, TEXT("Failed to get new GameplayPop actor from pool."));
				continue;
			}
			NewPop->ActivateGameplayPop(ActorPool, WidgetClass, Descriptor);
			USMObjectPool* DescriptorPool = DescriptorPools.FindChecked(PendingPop.PoolName);
			DescriptorPool->ReturnToPool(Descriptor);
		}
	}
}

void USMGameplayPopSubsystem::QueueActivateDamagePop(AActor* Instigator, const FVector& Location, int32 Damage, bool bCritical)
{
	USMGameplayDamagePopDescriptor* Descriptor = Cast<USMGameplayDamagePopDescriptor>(DescriptorPools[DamagePopName]->GetOrCreate());
	Descriptor->Instigator = Instigator;
	Descriptor->Location = Location;
	Descriptor->Damage = Damage;
	Descriptor->bCritical = bCritical;

	QueueActivatePop(DamagePopName, Descriptor);
}

void USMGameplayPopSubsystem::QueueActivateItemPop(AActor* Instigator, const FVector& Location, const FText& DisplayName)
{
	USMGameplayItemPopDescriptor* Descriptor = Cast<USMGameplayItemPopDescriptor>(DescriptorPools[ItemPopName]->GetOrCreate());
	Descriptor->Instigator = Instigator;
	Descriptor->Location = Location;
	Descriptor->DisplayName = DisplayName;

	QueueActivatePop(ItemPopName, Descriptor);
}

void USMGameplayPopSubsystem::QueueActivateBuffPop(AActor* Instigator, const FVector& Location, const FText& DisplayName, bool bIsDebuff)
{
	USMGameplayBuffPopDescriptor* Descriptor = Cast<USMGameplayBuffPopDescriptor>(DescriptorPools[BuffPopName]->GetOrCreate());
	Descriptor->Instigator = Instigator;
	Descriptor->Location = Location;
	Descriptor->DisplayName = DisplayName;
	Descriptor->bIsDebuff = bIsDebuff;

	QueueActivatePop(BuffPopName, Descriptor);
}

void USMGameplayPopSubsystem::QueueActivateTilePop(AActor* Instigator, const FVector& Location, int32 Count)
{
	USMGameplayTilePopDescriptor* Descriptor = Cast<USMGameplayTilePopDescriptor>(DescriptorPools[TilePopName]->GetOrCreate());
	Descriptor->Instigator = Instigator;
	Descriptor->Location = Location;
	Descriptor->Count = Count;

	QueueActivatePop(TilePopName, Descriptor);
}

void USMGameplayPopSubsystem::QueueActivatePop(const FName& PoolName, USMGameplayPopDescriptor* Descriptor)
{
	if (!Descriptor->Instigator)
	{
		UE_LOG(LogStereoMixGameplayPop, Warning, TEXT("Failed to activate GameplayPop. Instigator is nullptr."));
		return;
	}
	PendingPops.Add({ PoolName, Descriptor });
}

void USMGameplayPopSubsystem::InitializeActorPool()
{
	auto CreatePool = [this](const FName PoolName) {
		if (ActorPools.Find(PoolName))
		{
			return false;
		}

		const int32 PoolSize = InitialPoolSize;
		USMActorPool* NewPool = NewObject<USMActorPool>(this);
		NewPool->Initialize(ASMGameplayPop::StaticClass(), PoolSize);
		ActorPools.Add(PoolName, NewPool);

		UE_LOG(LogStereoMixGameplayPop, Verbose, TEXT("Created new GameplayPop Actor pool for %s. (InitialSize: %d)"), *PoolName.ToString(), PoolSize);
		return true;
	};

	CreatePool(DamagePopName);
	CreatePool(ItemPopName);
	CreatePool(BuffPopName);
	CreatePool(TilePopName);
}

void USMGameplayPopSubsystem::InitializeDescriptorPool()
{
	auto CreatePool = [this](const FName PoolName, const TSubclassOf<USMGameplayPopDescriptor>& DescriptorClass) {
		if (DescriptorPools.Find(PoolName))
		{
			return false;
		}

		const int32 PoolSize = InitialPoolSize;
		USMObjectPool* NewPool = NewObject<USMObjectPool>(this);
		NewPool->Initialize(DescriptorClass, PoolSize);
		DescriptorPools.Add(PoolName, NewPool);

		UE_LOG(LogStereoMixGameplayPop, Verbose, TEXT("Created new GameplayPop Descriptor pool for %s. (InitialSize: %d)"), *PoolName.ToString(), PoolSize);
		return true;
	};

	CreatePool(DamagePopName, USMGameplayDamagePopDescriptor::StaticClass());
	CreatePool(ItemPopName, USMGameplayItemPopDescriptor::StaticClass());
	CreatePool(BuffPopName, USMGameplayBuffPopDescriptor::StaticClass());
	CreatePool(TilePopName, USMGameplayTilePopDescriptor::StaticClass());
}

ASMGameplayPop* USMGameplayPopSubsystem::CreateNewPopActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;

	uint64 NewPopID = PopID + 1;
	SpawnParams.Name = *FString::Printf(TEXT("GameplayPop_%llu"), NewPopID);

	ASMGameplayPop* NewPopActor = World->SpawnActor<ASMGameplayPop>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!NewPopActor)
	{
		UE_LOG(LogStereoMixGameplayPop, Error, TEXT("Failed to spawn new GameplayPop actor."));
		return nullptr;
	}

	PopID = NewPopID;
	NewPopActor->SetActorHiddenInGame(true);
	return NewPopActor;
}
