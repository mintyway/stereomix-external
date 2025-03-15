// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMTeam.h"
#include "Subsystems/WorldSubsystem.h"
#include "SMGameplayPopSubsystem.generated.h"

class USMObjectPool;
class USMActorPool;
class USMGameplayPopDescriptor;
class ASMGameplayPop;

/**
 * StereoMix Gameplay Pop Subsystem
 */
UCLASS(Config = Game, DefaultConfig, DisplayName = "StereoMix Gameplay Pop Subsystem")
class STEREOMIX_API USMGameplayPopSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	static USMGameplayPopSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	void QueueActivateDamagePop(AActor* Instigator, const FVector& Location, int32 Damage, bool bCritical);
	void QueueActivateItemPop(AActor* Instigator, const FVector& Location, const FText& DisplayName);
	void QueueActivateBuffPop(AActor* Instigator, const FVector& Location, const FText& DisplayName, bool bIsDebuff);
	void QueueActivateTilePop(AActor* Instigator, const FVector& Location, int32 Count);

protected:
	void QueueActivatePop(const FName& PoolName, USMGameplayPopDescriptor* Descriptor);

private:
	struct FSMPendingGameplayPop
	{
		FName PoolName;
		USMGameplayPopDescriptor* Descriptor;
	};

	void InitializeActorPool();
	void InitializeDescriptorPool();

	ASMGameplayPop* CreateNewPopActor();

	uint64 PopID = 0;

	UPROPERTY()
	TMap<FName, USMActorPool*> ActorPools;

	UPROPERTY()
	TMap<FName, USMObjectPool*> DescriptorPools;

	TMap<FName, UClass*> PopWidgetClassMap;
	TArray<FSMPendingGameplayPop> PendingPops;

	int32 InitialPoolSize;
	int32 MaxActivatePopsPerTick;
};
