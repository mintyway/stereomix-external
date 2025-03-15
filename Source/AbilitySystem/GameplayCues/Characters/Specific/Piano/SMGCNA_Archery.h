// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Archery.generated.h"

class ASMPianoCharacter;
class UFMODAudioComponent;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Archery : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_Archery();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	FVector Offset = FVector(10.0, 0.0, 22.0);

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;

	UPROPERTY(EditAnywhere, Category = "FX|VFX")
	TObjectPtr<UNiagaraSystem> IndicatorVFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> IndicatorVFXComponent;

	static const FName ChargeRatioParameterName;

	static const FName MaxDistanceParameterName;

	static const FName TeamParameterName;

	UPROPERTY()
	TWeakObjectPtr<ASMPianoCharacter> OwnerCharacter;

	float ChargeElapsedTime = 0.0f;

	float TimeToChargeLevel1 = 0.0f;

	float TimeToChargeLevel2 = 0.0f;

	float MaxDistance = 0.0f;
};
