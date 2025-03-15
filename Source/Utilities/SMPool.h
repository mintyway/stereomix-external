// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPool.generated.h"

class UObject;

DECLARE_LOG_CATEGORY_EXTERN(LogStereoMixObjectPool, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogStereoMixActorPool, Log, All);

UINTERFACE()
class USMPoolInterface : public UInterface
{
	GENERATED_BODY()
};

class ISMPoolInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UClass* InClass, int32 InitialSize) = 0;

	template<typename T>
	T* GetOrCreate()
	{
		return CastChecked<T>(GetOrCreate());
	}

	virtual UObject* GetOrCreate() = 0;

	virtual void ReturnToPool(UObject* Object) = 0;

	virtual void Clear() = 0;

	virtual int32 GetAvailableCount() const = 0;
};

UCLASS(MinimalAPI)
class USMObjectPool : public UObject, public ISMPoolInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UClass* InObjectClass, int32 InitialSize) override;
	virtual UObject* GetOrCreate() override;
	virtual void ReturnToPool(UObject* Object) override;
	virtual void Clear() override;
	virtual int32 GetAvailableCount() const override;

private:
	FName CreateNewObjectName();

	UPROPERTY(Transient)
	TArray<UObject*> AvailableObjects;

	bool bInitialized = false;
	uint64 ObjectId = 0;
	UClass* ObjectClass = nullptr;
};

UCLASS()
class USMActorPool : public UObject, public ISMPoolInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UClass* InClass, int32 InitialSize) override;
	virtual UObject* GetOrCreate() override;
	virtual void ReturnToPool(UObject* Object) override;
	virtual void Clear() override;
	virtual int32 GetAvailableCount() const override;

private:
	FName CreateNewActorName();

	UPROPERTY(Transient)
	TArray<AActor*> AvailableActors;

	bool bInitialized = false;
	uint64 ActorId = 0;
	UClass* ActorClass = nullptr;
};
