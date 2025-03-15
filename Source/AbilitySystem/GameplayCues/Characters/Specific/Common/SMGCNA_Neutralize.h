// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "SMGCNA_Buff.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Neutralize.generated.h"

class ASMNoteBase;
class ASMCharacterBase;
class ASMPlayerCharacterBase;
class USMNeutralizeTimerComponent;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Neutralize : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_Neutralize();

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	void UpdateTimerRatio(float NewRatio);

	void UpdateFloorCircle();

	/** 무력화 타이머를 활성화합니다. */
	void ActivateTimer();

	/** 무력화 타이머를 비활성화합니다. */
	void DeactivateTimer();

	void OnHeldStateEntryCallback();

	void OnHeldStateExitCallback();

	/** 무력화 타이머의 모양을 기본 상태로 리셋합니다.*/
	void ResetFloorCircleState();

	/** 해당 캐릭터가 소유하고 있는 노트를 반환하는 랩핑함수입니다. */
	static ASMNoteBase* GetNote(const ASMCharacterBase* InCharacter);

	/** 잡기의 최대 사거리 기준 적용되야하는 스케일 값을 반환합니다. 이는 적절한 오프셋도 포함되어있습니다. */
	float GetMaxDistanceScale();

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "NeutralizeTimer")
	TObjectPtr<UStaticMeshComponent> NeutralizeTimerMesh;

	/** FloorCircle과 크기를 맞추기 위한 Timer 스케일 상수값입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float DefaultTimerScale = 1.75f;

	UPROPERTY()
	TObjectPtr<USMNeutralizeTimerComponent> OwnerNeutralizeTimerComponent;

	TWeakObjectPtr<ASMCharacterBase> OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> EndVFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ScreenVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> StartSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> EndSFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ScreenVFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;

	float HoldDistance = 0.0f;

	/** 잡기 사거리 기준 몇을 곱해서 스케일 보간 시작 거리를 구할지 정합니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float ScaleInterpolationStartMultiplier = 2.0f;

	/** 잡기 사거리 기준 몇을 곱해서 스케일 보간 시작 거리를 구할지 정합니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float ScaleInterpolationEndMultiplier = 2.25f;

	float ScaleInterpolationStartDistance = 0.0f;

	float ScaleInterpolationEndDistance = 0.0f;

	/** 조금 더 원을 작게 표현해 확실히 잡을 수 있는 경우만 표시하도록 하는 오프셋입니다. */
	float HoldDistanceOffset = 30.0f;

	TWeakObjectPtr<ASMCharacterBase> LocalCharacter;

	TWeakObjectPtr<UStaticMeshComponent> FloorCircleComponent;

	FName FloorCircleHoldableRangeParameterName = TEXT("Lerp");

	FName FloorCircleExpandStateColorParameterName = TEXT("Color");
};
