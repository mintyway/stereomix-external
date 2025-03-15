// Copyright Surround, Inc. All Rights Reserved.


#include "SMScreenIndicatorWidget.h"

#include "GameFramework/Character.h"
#include "StereoMixLog.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"

USMScreenIndicatorWidget::USMScreenIndicatorWidget()
{
}

void USMScreenIndicatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (UCanvasPanelSlot* IndicatorSlot = Cast<UCanvasPanelSlot>(Indicator->Slot))
	{
		IndicatorSlot->SetAnchors(FAnchors(0.f, 0.f));
	}
	else
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("Indicator Widget must have a CanvasPanelSlot."));
	}
}

void USMScreenIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (TargetActor.Get())
	{
		NativeUpdateIndicator(MyGeometry);
	}
}

void USMScreenIndicatorWidget::NativeUpdateIndicator(const FGeometry& MyGeometry)
{
	UCanvasPanelSlot* BaseSlot = Cast<UCanvasPanelSlot>(Indicator->Slot);
	if (!BaseSlot)
	{
		return;
	}

	APlayerController* SourcePlayerController = GetOwningPlayer();
	if (!SourcePlayerController)
	{
		return;
	}

	if (!TargetActor.Get())
	{
		return;
	}

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(SourcePlayerController);
	const FVector2D ScreenCenter(ViewportSize.X / 2.0, ViewportSize.Y / 2.0);

	// 타겟의 월드 좌표를 스크린좌표로 변환합니다.
	const ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	FVector TargetLocation = TargetActor->GetActorLocation();
	if (TargetCharacter)
	{
		USkeletalMeshComponent* TargetMesh = TargetCharacter->GetMesh();
		if (ensureAlways(TargetMesh))
		{
			const FName SocketName = TEXT("Trail-Point");
			TargetLocation = TargetMesh->GetSocketLocation(SocketName);
		}
	}

	FVector2D TargetScreenLocation;
	if (!SourcePlayerController->ProjectWorldLocationToScreen(TargetLocation, TargetScreenLocation, true))
	{
		// 카메라 뒤에 위치하는 경우 수동으로 계산합니다.
		TargetScreenLocation = GetScreenLocationForTargetBehindCamera(TargetLocation, ViewportSize, ScreenCenter);
	}

	// 오프셋을 고려하여 경계면을 설정합니다.
	const float ViewportOffset = ViewportSize.Y / VerticalOffsetRatio;
	const FVector2D MinBoundary = FVector2D(ViewportOffset, ViewportOffset);
	const FVector2D MaxBoundary = FVector2D(ViewportSize.X - ViewportOffset, ViewportSize.Y - ViewportOffset);

	ESMIndicatorDisplayState DisplayState = IsInBounds(TargetScreenLocation, MinBoundary, MaxBoundary) ? ESMIndicatorDisplayState::InScreen : ESMIndicatorDisplayState::OffScreen;

	if (DisplayState == ESMIndicatorDisplayState::OffScreen)
	{
		TargetScreenLocation = GetOffScreenIndicatorScreenLocation(MyGeometry, TargetScreenLocation, ViewportSize, ScreenCenter, ViewportOffset, MinBoundary, MaxBoundary);
	}

	// 스크린 좌표를 위젯에 적용하기 위해 위젯의 로컬 좌표로 변환하고 적용합니다.
	FVector2D LocalCoordinate;
	USlateBlueprintLibrary::ScreenToWidgetLocal(SourcePlayerController, MyGeometry, TargetScreenLocation, LocalCoordinate);
	BaseSlot->SetPosition(LocalCoordinate);

	// 스크린 중앙에서 타겟을 향하는 방향으로 인디케이터를 회전시킵니다.
	FVector2D Direction = (TargetScreenLocation - ScreenCenter).GetSafeNormal();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X)) + 90.0f;

	BP_UpdateIndicator(DisplayState, Angle);
}

bool USMScreenIndicatorWidget::IsInBounds(const FVector2D& TargetScreenLocation, const FVector2D& MinBoundary, const FVector2D& MaxBoundary)
{
	auto IsAxisInBounds = [](double Min, double Max, double Value) {
		return Min <= Value && Value <= Max;
	};

	const bool bIsInBoundsX = IsAxisInBounds(MinBoundary.X, MaxBoundary.X, TargetScreenLocation.X);
	const bool bIsInBoundsY = IsAxisInBounds(MinBoundary.Y, MaxBoundary.Y, TargetScreenLocation.Y);
	const bool Result = bIsInBoundsX && bIsInBoundsY;
	return Result;
}

FVector2D USMScreenIndicatorWidget::GetOffScreenIndicatorScreenLocation(const FGeometry& InGeometry, const FVector2D& TargetScreenLocation, const FVector2D& ViewportSize, const FVector2D& ScreenCenter, float ViewportOffset, const FVector2D& MinBoundary, const FVector2D& MaxBoundary)
{
	// 스크린 좌표의 중심에서 타겟을 향한 방향과 타겟의 위치를 저장합니다.
	const FVector2D Direction = (TargetScreenLocation - ScreenCenter).GetSafeNormal();
	if (Direction == FVector2D::ZeroVector)
	{
		return FVector2D::ZeroVector;
	}

	FVector2D OffScreenLocation = TargetScreenLocation;

	// CheckX를 통해 사용되는 축이 X축인지 Y축인지 구별하는 람다식입니다.
	auto CalculateIntersection = [&ScreenCenter, &Direction, &OffScreenLocation](bool CheckX, float ScreenCenterCoord, float DirectionCoord, float BoundaryCoord, float OtherMinBoundaryCoord, float OtherMaxBoundaryCoord) {
		// 한 축의 중심을 기준으로 한 축의 경계까지의 길이를 구하고 이를 한 축의 방향으로 나눕니다. 이를 통해 어떤 길이를 가질떄 왼쪽 경계에 도달할 수 있는지 구할 수 있습니다.
		const float DistanceToBoundary = (BoundaryCoord - ScreenCenterCoord) / DirectionCoord;
		FVector2D Intersection = ScreenCenter + (Direction * DistanceToBoundary);

		// 반대되는 축의 최소 경계와 최대 경계 사이에 존재하는지 확인합니다. 이 사이에 존재한다면 유효한 교점입니다.
		const float IntersectionCoord = CheckX ? Intersection.Y : Intersection.X;
		if (OtherMinBoundaryCoord <= IntersectionCoord && IntersectionCoord <= OtherMaxBoundaryCoord)
		{
			OffScreenLocation = Intersection;
		}
	};

	// 가로, 세로 순서로 교점을 계산하고 유효하면 ClampedTargetScreenLocation에 저장합니다. 가로, 세로에서 모두 유효한 교점이 나온다면 세로측 교점을 우선시하게됩니다. (사실상 같은 값)
	if (Direction.X < 0)
	{
		CalculateIntersection(true, ScreenCenter.X, Direction.X, MinBoundary.X, MinBoundary.Y, MaxBoundary.Y);
	}
	else if (Direction.X > 0)
	{
		CalculateIntersection(true, ScreenCenter.X, Direction.X, MaxBoundary.X, MinBoundary.Y, MaxBoundary.Y);
	}

	if (Direction.Y < 0)
	{
		CalculateIntersection(false, ScreenCenter.Y, Direction.Y, MinBoundary.Y, MinBoundary.X, MaxBoundary.X);
	}
	else if (Direction.Y > 0)
	{
		CalculateIntersection(false, ScreenCenter.Y, Direction.Y, MaxBoundary.Y, MinBoundary.X, MaxBoundary.X);
	}

	return OffScreenLocation;
}

FVector2D USMScreenIndicatorWidget::GetScreenLocationForTargetBehindCamera(const FVector& TargetLocation, const FVector2D& ViewportSize, const FVector2D& ScreenCenter) const
{
	APlayerController* SourcePlayerController = GetOwningPlayer();
	if (!SourcePlayerController)
	{
		return FVector2D::ZeroVector;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	SourcePlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector ToTargetDirection = (TargetLocation - CameraLocation).GetSafeNormal();

	const FVector Forward = CameraRotation.Vector();
	const FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector Up = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	const float ForwardDot = FVector::DotProduct(Forward, ToTargetDirection);
	const float RightDot = FVector::DotProduct(Right, ToTargetDirection);
	const float UpDot = FVector::DotProduct(Up, ToTargetDirection);

	FVector2D ScreenLocation;
	// 카메라의 뒤쪽에 있는 경우, 반대 방향으로 스크린 경계에 배치합니다.
	if (ForwardDot <= 0.0f)
	{
		FVector2D OffScreenDirection = FVector2D(RightDot, -UpDot).GetSafeNormal();
		ScreenLocation = ScreenCenter + OffScreenDirection * ScreenCenter.X;

		return ScreenLocation;
	}

	// 만약이지만 정상적인 상황을 처리해야할 경우가 존재할 수 있어 이에 대한 처리입니다.
	ScreenLocation.X = ScreenCenter.X + (RightDot / ForwardDot) * ScreenCenter.X;
	ScreenLocation.Y = ScreenCenter.Y - (UpDot / ForwardDot) * ScreenCenter.Y;

	return ScreenLocation;
}

void USMScreenIndicatorWidget::SetTarget(AActor* InTargetActor)
{
	TargetActor = InTargetActor;
	BP_SetTarget(InTargetActor);
}
