#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowItem.generated.h"

class ASMTile;
class ASMItemBase;
class ASMThrowableItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrowItem);

UCLASS(Abstract)
class STEREOMIX_API ASMThrowItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowItem();

	virtual void PostInitializeComponents() override;

	/** 아이템을 생성할 수 있는 위치를 초기화 후 다시 찾습니다. */
	void UpdateAvailableSpawnLocations();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	static FVector CalculateHalfExtent(int32 Columns, int32 Rows, float Height);

	static FVector GetBoxCenter(const FVector& ActorLocation, float ZOffset);

	bool IsLocationAvailableForSpawn(const ASMTile* Tile, const FVector& Location) const;

	// 이거를 Server Reliable로 사용한 이유가..?

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ScheduleThrowItem();

	UFUNCTION(BlueprintCallable)
	void ThrowItem();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnThrowItem();

	void InternalThrowItem(const FVector& TargetLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	TArray<TSubclassOf<ASMItemBase>> ThrowingItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	TSubclassOf<ASMThrowableItem> ThrowableItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	int32 ThrowCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	float ThrowInterval = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	float SpawnOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	int32 MaxThrowTilesRow = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	int32 MaxThrowTilesColumn = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	float ParabolaHeight = 1000.0f;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnThrowItem OnThrowItem;

private:
	TArray<FVector> AvailableSpawnLocations;
};
