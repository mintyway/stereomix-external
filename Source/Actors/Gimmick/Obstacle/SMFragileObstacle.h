﻿// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SMObstacleBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "SMFragileObstacle.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FSMFragileObstacleDurabilityThresholdData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DurabilityRatio = 0.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "4.0"))
	float DamageParam = 0.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "4.0"))
	float CrackStep = 0.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontage;
};

UCLASS(Abstract)
class STEREOMIX_API ASMFragileObstacle : public ASMObstacleBase, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMFragileObstacle();

	virtual void PreInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

	virtual void PostInitializeComponents() override;

	/** 장애물을 처음 상태로 복구할때 사용합니다. */
	UFUNCTION(Server, Reliable)
	void ServerRestoreObstacle();

	UFUNCTION(Server, Reliable)
	void ServerSetCurrentDurability(float NewCurrentDurability);

	UFUNCTION(Server, Reliable)
	void ServerSetMaxDurability(float NewMaxDurability);

	virtual void SetCollisionEnabled(bool bNewIsCollisionEnabled) override;

	virtual void UnloadObstacle() override;

	virtual AActor* GetLastAttacker() const override { return LastAttacker.Get(); }

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount, const FGameplayTagContainer& AggregatedSourceTags) override;

	UFUNCTION(Client, Unreliable)
	virtual void ClientPlayCrackSound(float Step = 0.0f);

	virtual bool CanIgnoreAttack() const override { return false; }

	virtual bool IsObstacle() const override { return true; }

protected:
	UFUNCTION()
	void OnRep_CurrentDurability();

	UFUNCTION()
	void OnRep_Durability();

	UPROPERTY(ReplicatedUsing = "OnRep_CurrentDurability")
	float CurrentDurability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design", ReplicatedUsing = "OnRep_Durability")
	float Durability = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UAnimMontage> DefaultHitAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere, Category = "Design|Sound")
	TObjectPtr<UFMODEvent> CrackSound;

	/** 좌측에는 퍼센트를 정규화한 값을 넣고, 우측에는 해당하는 메시를 넣어주세요. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	TArray<FSMFragileObstacleDurabilityThresholdData> DurabilityThresholds;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttacker;

	float LatestDurabilityRatioLevel = 1.0f;

private:
	void UpdateVisualBasedOnDurability();
};
