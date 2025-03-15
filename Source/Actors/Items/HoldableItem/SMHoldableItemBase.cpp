// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItemBase.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Common/SMGA_Hold.h"
#include "Components/SphereComponent.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMBasicShapeFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMCollision.h"


const FName ASMHoldableItemBase::HICName = TEXT("HIC");
const FName ASMHoldableItemBase::FloorCircleHoldableRangeParameterName = TEXT("Lerp");
const FName ASMHoldableItemBase::FloorCircleExpandStateColorParameterName = TEXT("Color");

ASMHoldableItemBase::ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Super::SetReplicateMovement(true);

	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::HoldableItem);

	HIC = CreateDefaultSubobject<USMHoldInteractionComponent>(HICName);

	FloorCircleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorCircleMeshComponent"));
	FloorCircleMeshComponent->SetupAttachment(RootComponent);
	FloorCircleMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	FloorCircleMeshComponent->SetStaticMesh(USMBasicShapeFunctionLibrary::GetPlaneMesh());
}

void ASMHoldableItemBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetNetMode() != NM_DedicatedServer)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < FloorCircleMeshComponent->GetNumMaterials(); ++MaterialIndex)
		{
			FloorCircleMeshComponent->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
		}

		InitHoldDistance();
	}
}

void ASMHoldableItemBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateFloorCircle();
}

void ASMHoldableItemBase::InitHoldDistance()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	const USMAbilitySystemComponent* LocalASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(LocalPawn.Get());
	if (const USMGA_Hold* HoldGAInstance = LocalASC ? LocalASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldDistance = HoldGAInstance->GetMaxDistance();
		ScaleInterpolationStartDistance = HoldDistance * ScaleInterpolationStartMultiplier;
		ScaleInterpolationEndDistance = HoldDistance * ScaleInterpolationEndMultiplier;

		SetActorTickEnabled(true);
	}
	else
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::InitHoldDistance));
	}
}

float ASMHoldableItemBase::GetMaxDistanceScale()
{
	const float HoldDistanceMaxScale = ((HoldDistance - HoldDistanceOffset) / 100.0f) * 2.0f;
	return HoldDistanceMaxScale > 1.0f ? HoldDistanceMaxScale : 1.0f;
}

void ASMHoldableItemBase::UpdateFloorCircle()
{
	if (!FloorCircleMeshComponent || !LocalPawn.IsValid())
	{
		return;
	}

	// 잡기 가능한 경우 색을 FloorCircle의 색을 바꿉니다.
	const float DistanceToOwner = FVector::Dist2D(LocalPawn->GetActorLocation(), GetActorLocation());
	const float HoldableRangeValue = DistanceToOwner <= (HoldDistance - HoldDistanceOffset) ? 1.0f : 0.0f; // 1은 붉은색입니다.
	FloorCircleMeshComponent->SetScalarParameterValueOnMaterials(FloorCircleHoldableRangeParameterName, HoldableRangeValue);

	// 스케일의 보간값을 구합니다.
	const float InterpolationDifference = ScaleInterpolationEndDistance - ScaleInterpolationStartDistance;
	const float InterpolationAlpha = FMath::Clamp(1.0f - ((DistanceToOwner - ScaleInterpolationStartDistance) / InterpolationDifference), 0.0f, 1.0f);
	FloorCircleMeshComponent->SetScalarParameterValueOnMaterials(FloorCircleExpandStateColorParameterName, InterpolationAlpha); // 1로 갈수록 흰색 0으로 갈 수록 검정색입니다.

	// 보간값을 통해 실제로 적용되야할 최종 스케일을 구하고 적용합니다.
	const float MaxScale = GetMaxDistanceScale() * InterpolationAlpha;
	const float FinalScale = MaxScale > DefaultScale ? MaxScale : DefaultScale;
	FloorCircleMeshComponent->SetRelativeScale3D(FVector(FinalScale));
}
