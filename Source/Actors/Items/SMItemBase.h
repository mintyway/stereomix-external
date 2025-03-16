// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "SMItemBase.generated.h"

class UNiagaraSystem;
class USMItemDataAsset;
class USphereComponent;
class UNiagaraComponent;

DECLARE_DELEGATE(FOnUsedItemSignature);

UCLASS(Abstract)
class STEREOMIX_API ASMItemBase : public AActor
{
	GENERATED_BODY()

public:
	ASMItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 아이템을 활성화합니다. 서브클래스에서 이를 상속하고 구현해줘야합니다. 서버에서만 호출해야합니다. */
	virtual void ActivateItem(AActor* InActivator);

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void DeactivateItem();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayNiagara(UNiagaraSystem* NiagaraSystem, FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Category = "Item")
	USMItemDataAsset* GetDataAsset() const { return DataAsset; }

	UFUNCTION(BlueprintPure)
	const FGameplayTagContainer& GetItemTags() const;

	FOnUsedItemSignature OnUsedItem;

	UPROPERTY()
	TObjectPtr<AActor> Activator;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USphereComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<USMItemDataAsset> DataAsset;
};
