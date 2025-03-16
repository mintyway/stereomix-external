// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "SMGCNA_Buff.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Immune.generated.h"

class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Immune : public ASMGCNA_Buff
{
	GENERATED_BODY()

public:
	ASMGCNA_Immune();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> ImmuneMaterial;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> ImmuneOverlayMaterial;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> WeaponImmuneMaterial;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> WeaponImmuneOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> EndVFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ScreenVFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ScreenVFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;
};
