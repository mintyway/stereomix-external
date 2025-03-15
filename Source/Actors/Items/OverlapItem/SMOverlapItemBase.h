// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actors/Items/SMItemBase.h"
#include "SMOverlapItemBase.generated.h"

class UNiagaraSystem;
class UGameplayEffect;

UCLASS(Abstract)
class STEREOMIX_API ASMOverlapItemBase : public ASMItemBase
{
	GENERATED_BODY()

public:
	ASMOverlapItemBase();

	virtual void PostInitializeComponents() override;

	virtual void DeactivateItem() override;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> GE;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float ActivateDelay = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> OnActivateFX;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> OnDestroyFX;

private:
	FGameplayTagContainer UnavailableTags;
};
