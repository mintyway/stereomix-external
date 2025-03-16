// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMChangeAttributeItem.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS(Abstract)
class STEREOMIX_API ASMChangeAttributeItem : public AActor
{
	GENERATED_BODY()

public:
	ASMChangeAttributeItem();

protected:
	virtual void PostInitializeComponents() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetHidden(bool bNewHidden);

	void HealRespawnTimerCallback();

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "SpawnerMesh")
	TObjectPtr<UStaticMeshComponent> SpawnerMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "ItemMesh")
	TObjectPtr<UStaticMeshComponent> ItemMeshComponent;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> GE;

	UPROPERTY(EditAnywhere, Category = "Design")
	float RespawnTime = 10.0f;
};
