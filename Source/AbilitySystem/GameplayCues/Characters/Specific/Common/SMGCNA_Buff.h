// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Buff.generated.h"

class USMBuffDataAsset;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Buff : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_Buff();

	UFUNCTION(BlueprintCallable, Category = "Buff")
	const USMBuffDataAsset* GetBuffData() const { return BuffData.Get(); }

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UFUNCTION(BlueprintCallable, Category = "Buff")
	void NotifyBuffAdd(AActor* Target, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Buff")
	void NotifyBuffRemove(AActor* Target) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	TObjectPtr<USMBuffDataAsset> BuffData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> LoopVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> OverlayMaterial;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LoopVFXComponent;

	FFMODEventInstance MusicEventInstance;
};
