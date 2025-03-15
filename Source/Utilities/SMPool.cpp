// Copyright Surround, Inc. All Rights Reserved.

#include "SMPool.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPool)

DEFINE_LOG_CATEGORY(LogStereoMixObjectPool);
DEFINE_LOG_CATEGORY(LogStereoMixActorPool);

void USMObjectPool::Initialize(UClass* InObjectClass, int32 InitialSize)
{
	check(InObjectClass);
	check(!bInitialized);
	bInitialized = true;

	ObjectClass = InObjectClass;
	AvailableObjects.Reserve(InitialSize);

	for (int32 i = 0; i < InitialSize; ++i)
	{
		UObject* Object = NewObject<UObject>(this, ObjectClass);
		check(Object);
		ReturnToPool(Object);
	}
	UE_LOG(LogStereoMixObjectPool, Verbose, TEXT("Pool initialized. (Class=%s, InitialSize=%d"), *GetNameSafe(ObjectClass), InitialSize);
}

UObject* USMObjectPool::GetOrCreate()
{
	check(bInitialized);

	UObject* Object;
	if (AvailableObjects.Num() > 0)
	{
		Object = AvailableObjects.Pop();
	}
	else
	{
		Object = NewObject<UObject>(this, ObjectClass);
		UE_LOG(LogStereoMixObjectPool, Verbose, TEXT("No available object in the pool. Creating new object -> %s"), *GetNameSafe(Object));
	}
	check(Object);
	UE_LOG(LogStereoMixObjectPool, VeryVerbose, TEXT("Get object from the pool -> %s"), *GetNameSafe(Object));
	return Object;
}

void USMObjectPool::ReturnToPool(UObject* Object)
{
	check(bInitialized);
	if (!Object)
	{
		return;
	}
	AvailableObjects.Add(Object);
	UE_LOG(LogStereoMixObjectPool, VeryVerbose, TEXT("Return object to the pool -> %s"), *GetNameSafe(Object));
}

void USMObjectPool::Clear()
{
	check(bInitialized);
	AvailableObjects.Empty();
}

int32 USMObjectPool::GetAvailableCount() const
{
	check(bInitialized);
	return AvailableObjects.Num();
}

FName USMObjectPool::CreateNewObjectName()
{
	check(bInitialized);
	return FName(*FString::Printf(TEXT("%s_%llu"), *ObjectClass->GetName(), ObjectId++));
}

void USMActorPool::Initialize(UClass* InClass, int32 InitialSize)
{
	check(InClass);
	check(!bInitialized);
	bInitialized = true;

	ActorClass = InClass;
	AvailableActors.Reserve(InitialSize);

	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnParams;

	for (int32 i = 0; i < InitialSize; ++i)
	{
		// SpawnParams.Name = CreateNewActorName();
		AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FTransform::Identity, SpawnParams);
		check(NewActor);
		ReturnToPool(NewActor);
	}
	UE_LOG(LogStereoMixActorPool, Verbose, TEXT("Actor Pool initialized. (Class=%s, InitialSize=%d"), *GetNameSafe(ActorClass), InitialSize);
}

UObject* USMActorPool::GetOrCreate()
{
	check(bInitialized);

	AActor* Actor;
	if (AvailableActors.Num() > 0)
	{
		Actor = AvailableActors.Pop();
	}
	else
	{
		UWorld* World = GetWorld();
		check(World);

		FActorSpawnParameters SpawnParams;
		// SpawnParams.Name = CreateNewActorName();
		Actor = World->SpawnActor<AActor>(ActorClass, FTransform::Identity, SpawnParams);
		UE_LOG(LogStereoMixActorPool, Verbose, TEXT("No available actor in the pool. Creating new actor -> %s"), *GetNameSafe(Actor));
	}
	check(Actor);
	UE_LOG(LogStereoMixActorPool, VeryVerbose, TEXT("Get actor from the pool -> %s"), *GetNameSafe(Actor));
	return Actor;
}

void USMActorPool::ReturnToPool(UObject* Object)
{
	check(bInitialized);
	if (!Object)
	{
		return;
	}
	AvailableActors.Add(Cast<AActor>(Object));
	UE_LOG(LogStereoMixActorPool, VeryVerbose, TEXT("Return actor to the pool -> %s"), *GetNameSafe(Object));
}

void USMActorPool::Clear()
{
	check(bInitialized);
	for (auto Actor : AvailableActors)
	{
		Actor->Destroy();
	}
	AvailableActors.Empty();
}

int32 USMActorPool::GetAvailableCount() const
{
	check(bInitialized);
	return AvailableActors.Num();
}

FName USMActorPool::CreateNewActorName()
{
	check(bInitialized);
	return FName(*FString::Printf(TEXT("%s_%llu"), *ActorClass->GetName(), ActorId++));
}
