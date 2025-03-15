// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponentPoolMethodEnum.h"
#include "SMObstacleBase.generated.h"

class UFMODEvent;
class UNiagaraSystem;
class UNiagaraComponent;
class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMObstacleBase : public AActor
{
	GENERATED_BODY()

public:
	ASMObstacleBase();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void SetCollisionEnabled(bool bNewIsCollisionEnabled);

	virtual void UnloadObstacle();

	UFUNCTION(Client, Unreliable)
	virtual void ClientPlayHitSound(float Step = 0.0f);

	FORCEINLINE float GetDestroyEffectDuration() const { return DestroyEffectDuration; }

protected:
	UFUNCTION(Server, Reliable)
	void ServerTeleportToSafeLocation();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCollisionPreset(FName NewCollisionProfileName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCollisionEnabled(bool bNewIsCollisionEnabled);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMeshAndNiagaraSystem(UStaticMesh* NewMesh, UNiagaraSystem* NewNiagaraSystem, ENCPoolMethod PoolMethod = ENCPoolMethod::ManualRelease);

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint8 bSpawnImmediately:1 = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	float ObjectMaterial = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Sound")
	TObjectPtr<UFMODEvent> HitSound;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	int32 EffectParameterX = 1;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 EffectParameterY = 1; 

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 EffectParameterZ = 1;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float SpawnPushBackMagnitude = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float SpawnDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.01"))
	float PreSpawnEffectDuration = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.01"))
	float SpawnEffectDuration = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.01"))
	float DestroyEffectDuration = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> PreSpawnEffect;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY()
	TObjectPtr<UStaticMesh> OriginalMesh;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> OriginalNiagaraSystem;

	FVector OriginalNiagaraSystemLocation;

	FRotator OriginalNiagaraSystemRotation;
};
