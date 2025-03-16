// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enum/SMRoundState.h"

#include "SMItemSpawner.generated.h"

class UWidgetComponent;
class ASMItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRemainingCooldownChangedSignature, float, RemainingCooldownSeconds, float, RemainingCooldownPercent);

UCLASS()
class STEREOMIX_API ASMItemSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASMItemSpawner();
	
	FRemainingCooldownChangedSignature OnRemainingCooldownChanged;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 스폰 타이머를 시작합니다. */
	UFUNCTION()
	void SpawnTimerStart();

	/** 스폰 타이머가 만료될때 호출됩니다. */
	void SpawnTimerCallback();

	/** 아이템을 스폰합니다. */
	UFUNCTION()
	void SpawnItem();

	void OnUsedItem();

	UFUNCTION()
	void OnDestroyedItems(AActor* DestroyedActor);

	/** 스포너의 상태를 초기화합니다. Item 포인터를 null로, 델리게이트를 해제합니다. */
	void ResetSpawnerState();
	
	void UpdateCooldown(float DeltaTime);
	
	void OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState);

	UPROPERTY(VisibleAnywhere, Category = "SceneComponent")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "MeshComponent")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "ItemSocketComponent")
	TObjectPtr<USceneComponent> ItemSocketComponent;

	UPROPERTY(VisibleAnywhere, Category = "ItemSpawnTimerWidgetComponent")
	TObjectPtr<UWidgetComponent> ItemSpawnTimerWidgetComponent;

	/** 스폰 시킬 아이템 클래스입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<ASMItemBase> SpawnItemClass;

	UPROPERTY()
	TObjectPtr<ASMItemBase> Item;

	/** 스폰까지 소요되는 시간을 나타냅니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float SpawnCooldown = 10.0f;

	/** 게임 시작 즉시 스폰할지 여부입니다. */
	UPROPERTY(EditAnywhere, Category = "Design", DisplayName = "즉시 스폰")
	uint32 bUseImmediatelySpawn:1 = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Design")
	float RemainingCooldownSeconds = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	ESMRoundState SpawnRound = ESMRoundState::InRound;
};
