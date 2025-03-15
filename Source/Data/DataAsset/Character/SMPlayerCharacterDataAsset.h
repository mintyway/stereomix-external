// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMCommonCharacterDataAsset.h"
#include "NiagaraSystem.h"
#include "Abilities/GameplayAbility.h"
#include "Data/SMTeam.h"
#include "Data/Ability/SMActiveAbility.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Data/SMControlData.h"
#include "SMPlayerCharacterDataAsset.generated.h"

class ASMPlayerCharacterBase;
class ULevelSequence;
class ASMNoteBase;
class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class ASMWeaponBase;
class USMScreenIndicatorWidget;

UCLASS(BlueprintType)
class STEREOMIX_API USMPlayerCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("Character")), GetFName());
	}

	const USMCommonCharacterDataAsset* GetCommonDataAsset() const { return CommonCharacterDataAsset; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class")
	TMap<ESMTeam, TSubclassOf<ASMPlayerCharacterBase>> CharacterClass;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<USMControlData> InputActions;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TMap<ESMActiveAbilityType, TSubclassOf<UGameplayAbility>> DefaultActiveAbilities;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> NoiseBreakIndicatorFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> SkillIndicatorVFX;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> NoiseBreakMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> NoiseBreakedMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> HoldMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> SkillMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> StunMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> LandingMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Montage")
	TMap<ESMTeam, TSubclassOf<ASMWeaponBase>> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, Category = "Note")
	TMap<ESMTeam, TSubclassOf<ASMNoteBase>> NoteClass;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> HitCameraShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> SkillHitCameraShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> NoiseBreakCameraShake;

protected:
	UPROPERTY(EditAnywhere, Category = "DataAsset")
	TObjectPtr<USMCommonCharacterDataAsset> CommonCharacterDataAsset;
};
