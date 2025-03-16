// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/SMUserWidget.h"

#include "SMScreenIndicatorWidget.generated.h"

class UImage;
class UCanvasPanel;
class UScaleBox;

UENUM(BlueprintType)
enum class ESMIndicatorDisplayState : uint8
{
	InScreen,
	OffScreen
};

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMScreenIndicatorWidget : public USMUserWidget
{
	GENERATED_BODY()

public:
	USMScreenIndicatorWidget();
	
	virtual void NativeOnInitialized() override;

	// 인디케이터가 가리킬 대상을 설정하고 인디케이터를 활성화합니다.
	void SetTarget(AActor* InTargetActor);
	
	UFUNCTION(BlueprintPure, Category = "Indicator")
	const AActor* GetTargetActor() const { return TargetActor.Get(); }
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Indicator", DisplayName = "On Set Target")
	void BP_SetTarget(AActor* InTargetActor);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Indicator", DisplayName = "On Update Indicator")
	void BP_UpdateIndicator(ESMIndicatorDisplayState DisplayState, float Angle);
	void NativeUpdateIndicator(const FGeometry& MyGeometry);
	
	/** 타겟의 스크린 위치가 경계 안에 존재하는지 확인합니다. */
	static bool IsInBounds(const FVector2D& TargetScreenLocation, const FVector2D& MinBoundary, const FVector2D& MaxBoundary);

	/** 오프 스크린 인디케이터 표시되야할 스크린 좌표를 반환합니다. 타겟이 스크린 경계 밖에 존재하는 경우에만 호출되야합니다. */
	static FVector2D GetOffScreenIndicatorScreenLocation(const FGeometry& InGeometry, const FVector2D& TargetScreenLocation, const FVector2D& ViewportSize, const FVector2D& ScreenCenter, float ViewportOffset, const FVector2D& MinBoundary, const FVector2D& MaxBoundary);

	/**
	 * 만약 카메라 뒤로 타겟이 넘어가는 경우 ProjectWorldLocationToScreen를 사용할 수 없어 수동으로 계산해야합니다. 이를 위한 함수입니다.
	 * 카메라 앞에 존재하는 경우도 계산은 가능하지만 FOV 고려가 되어있지 않아 부정확합니다. 따라서 카메라가 뒤로 넘어간 경우에만 사용하는 걸 권장합니다.
	 */
	FVector2D GetScreenLocationForTargetBehindCamera(const FVector& TargetLocation, const FVector2D& ViewportSize, const FVector2D& ScreenCenter) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> Indicator;

	/**
	 * 경계면을 기준으로 인디케이터가 나타나는 위치의 오프셋의 크기를 결정하는데 사용됩니다. 이 값은 뷰포트 크기의 세로사이즈를 나누는 값으로 쓰이며 이 나눈 값을 오프셋을 사용합니다.
	 * 결과적으로 작은 값을 쓸수록 오프셋이 커집니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Design")
	float VerticalOffsetRatio = 7.5f;

	UPROPERTY()
	TObjectPtr<const AActor> TargetActor;
};
