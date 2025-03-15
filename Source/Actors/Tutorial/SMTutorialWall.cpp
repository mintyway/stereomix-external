// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialWall.h"

#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "FunctionLibraries/SMBasicShapeFunctionLibrary.h"
#include "Utilities/SMCollision.h"


const FName ASMTutorialWall::WallOpacityParameterName = TEXT("Opacity");

ASMTutorialWall::ASMTutorialWall()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	ProximityBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProximityBoxComponent"));
	ProximityBoxComponent->SetupAttachment(RootComponent);
	ProximityBoxComponent->SetCollisionProfileName(SMCollisionProfileName::PlayerTrigger);
	ProximityBoxComponent->InitBoxExtent(FVector(MaxDistanceY, 50.0, 100.0));
	ProximityBoxComponent->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	ProximityBoxComponent->SetRelativeScale3D(FVector(1.0, 8.0, 1.0));
	ProximityBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPlayerBeginOverlap);
	ProximityBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnPlayerEndOverlap);

	WallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallComponent"));
	WallMeshComponent->SetupAttachment(RootComponent);
	WallMeshComponent->SetStaticMesh(USMBasicShapeFunctionLibrary::GetPlaneMesh());
	WallMeshComponent->SetCollisionProfileName(SMCollisionProfileName::BlockAll);
	WallMeshComponent->SetRelativeLocationAndRotation(FVector(0.0, 0.0, 150.0), FRotator(90.0, 0.0, 0.0));
	WallMeshComponent->SetRelativeScale3D(FVector(1.0, 8.0, 1.0));
}

void ASMTutorialWall::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProximityBoxComponent->SetBoxExtent(FVector(MaxDistanceY, 50.0, 100.0));

	for (int32 i = 0; i < WallMeshComponent->GetNumMaterials(); ++i)
	{
		WallMIDs.Add(WallMeshComponent->CreateAndSetMaterialInstanceDynamic(i));
	}

	SetOpacity(0.0f);
}

void ASMTutorialWall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateOpacity();
}

void ASMTutorialWall::SetOpacity(float Alpha)
{
	const float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	for (UMaterialInstanceDynamic* WallMID : WallMIDs)
	{
		if (WallMID)
		{
			WallMID->SetScalarParameterValue(WallOpacityParameterName, ClampedAlpha);
		}
	}
}

void ASMTutorialWall::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool BFromSweep, const FHitResult& SweepResult)
{
	if (ASMPlayerCharacterBase* OverlappedPlayerCharacter = Cast<ASMPlayerCharacterBase>(OtherActor); OverlappedPlayerCharacter && OverlappedPlayerCharacter->IsLocallyControlled())
	{
		CachedPlayerCharacter = OverlappedPlayerCharacter;
		SetActorTickEnabled(true);
	}
}

void ASMTutorialWall::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (const ASMPlayerCharacterBase* OverlappedPlayerCharacter = Cast<ASMPlayerCharacterBase>(OtherActor); OverlappedPlayerCharacter && OverlappedPlayerCharacter->IsLocallyControlled())
	{
		SetActorTickEnabled(false);
		CachedPlayerCharacter = nullptr;

		SetOpacity(0.0f);
	}
}

void ASMTutorialWall::UpdateOpacity()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		return;
	}

	const FVector SelfLocation = GetActorLocation();
	const FVector PlayerLocation = CachedPlayerCharacter->GetActorLocation();

	const float DistanceY = FMath::Abs(PlayerLocation.Y - SelfLocation.Y);
	const float Alpha = (FMath::Clamp(DistanceY, MinDistanceY, MaxDistanceY) - MinDistanceY) / (MaxDistanceY - MinDistanceY);
	SetOpacity(1.0f - Alpha);
}
