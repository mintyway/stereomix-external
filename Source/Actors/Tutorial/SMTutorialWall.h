// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMTutorialWall.generated.h"

class ASMPlayerCharacterBase;
class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMTutorialWall : public AActor
{
	GENERATED_BODY()

public:
	ASMTutorialWall();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	void SetOpacity(float Alpha);

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ProximityBoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> WallMeshComponent;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> WallMIDs;

	static const FName WallOpacityParameterName;

	TWeakObjectPtr<ASMPlayerCharacterBase> CachedPlayerCharacter;

	UPROPERTY(EditAnywhere, Category = "Design")
	float MaxDistanceY = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float MinDistanceY = 300.0f;

private:
	UFUNCTION()
	void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool BFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	void UpdateOpacity();
};
