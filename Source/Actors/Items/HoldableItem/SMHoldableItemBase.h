// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/SMItemBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "SMHoldableItemBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMHoldableItemBase : public ASMItemBase, public ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override { return HIC; }

	virtual void ActivateItemByNoiseBreak(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& TilesToBeCaptured) {}

	const static FName HICName;

protected:
	UPROPERTY(VisibleAnywhere, Category = "HoldInteraction")
	TObjectPtr<USMHoldInteractionComponent> HIC;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UStaticMeshComponent> FloorCircleMeshComponent;

	static const FName FloorCircleHoldableRangeParameterName;

	static const FName FloorCircleExpandStateColorParameterName;

	TWeakObjectPtr<APawn> LocalPawn;

	/** FloorCircle의 기본 스케일 상수값입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float DefaultScale = 1.75f;

	float HoldDistance = 0.0f;

	/** 조금 더 원을 작게 표현해 확실히 잡을 수 있는 경우만 표시하도록 하는 오프셋입니다. */
	float HoldDistanceOffset = 30.0f;

	/** 잡기 사거리 기준 몇을 곱해서 스케일 보간 시작 거리를 구할지 정합니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float ScaleInterpolationStartMultiplier = 2.0f;

	/** 잡기 사거리 기준 몇을 곱해서 스케일 보간 시작 거리를 구할지 정합니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float ScaleInterpolationEndMultiplier = 2.25f;

	float ScaleInterpolationStartDistance = 0.0f;

	float ScaleInterpolationEndDistance = 0.0f;

private:
	void InitHoldDistance();

	/** 잡기의 최대 사거리 기준 적용되야하는 스케일 값을 반환합니다. 이는 적절한 오프셋도 포함되어있습니다. */
	float GetMaxDistanceScale();

	void UpdateFloorCircle();
};
