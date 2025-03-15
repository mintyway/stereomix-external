// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/SMTeam.h"
#include "UI/Game/SMGameplayAttributeWidget.h"
#include "UI/Game/SMScreenIndicatorWidget.h"
#include "SMCommonCharacterDataAsset.generated.h"

struct FGameplayAttribute;
class UFMODEvent;
class UInputAction;
class ULevelSequence;
class UNiagaraSystem;
class USMNameplateWidget;
class UGameplayAbility;
class UGameplayEffect;


UCLASS()
class STEREOMIX_API USMCommonCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<USMNameplateWidget> NameplateWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<USMScreenIndicatorWidget> ScreenIndicatorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "GAS|Attribute")
	FGameplayAttribute HealthAttribute;

	UPROPERTY(EditAnywhere, Category = "GAS|Attribute")
	FGameplayAttribute MaxHealthAttribute;

	UPROPERTY(EditAnywhere, Category = "GAS|Attribute")
	FGameplayAttribute SkillGaugeAttribute;

	UPROPERTY(EditAnywhere, Category = "GAS|Attribute")
	FGameplayAttribute MaxSkillGaugeAttribute;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> StatInitGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> SkillGaugeGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> StaminaGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> DamageGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> StaminaHealGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> ResetStaminaGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TArray<TSubclassOf<UGameplayAbility>> DefaultGAs;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TArray<TSubclassOf<UGameplayAbility>> PassiveGAs;

	UPROPERTY(EditAnywhere, Category = "FX|VFX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> DefaultMoveTrailVFX;

	UPROPERTY(EditAnywhere, Category = "FX|VFX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> HoldMoveTrailVFX;

	UPROPERTY(EditAnywhere, Category = "FX|VFX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ImmuneMoveTrailVFX;

	UPROPERTY(EditAnywhere, Category = "FX|VFX")
	TObjectPtr<UNiagaraSystem> LandingDownVFX;

	UPROPERTY(EditAnywhere, Category = "FX|SFX")
	TObjectPtr<UFMODEvent> LandingDownSFX;

	UPROPERTY(EditAnywhere, Category = "FX|VFX")
	TObjectPtr<UNiagaraSystem> TeleportUpVFX;

	UPROPERTY(EditAnywhere, Category = "Indicator|Material")
	TObjectPtr<UMaterialInterface> DirectionIndicatorMaterial;

	UPROPERTY(EditAnywhere, Category = "Indicator|Material")
	TObjectPtr<UMaterialInterface> FloorCircleMaterial;

	UPROPERTY(EditAnywhere, Category = "Animation|Curve")
	TObjectPtr<UCurveFloat> BlinkCurve;

	UPROPERTY(EditAnywhere, Category = "Animation|Curve")
	TObjectPtr<UCurveFloat> MouthCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Emoticon|Input")
	TObjectPtr<UInputAction> Emoticon1Action;

	UPROPERTY(EditDefaultsOnly, Category = "Emoticon|Input")
	TObjectPtr<UInputAction> Emoticon2Action;

	UPROPERTY(EditDefaultsOnly, Category = "Emoticon|Input")
	TObjectPtr<UInputAction> Emoticon3Action;

	UPROPERTY(EditDefaultsOnly, Category = "Emoticon|Widget")
	TSubclassOf<UUserWidget> EmoticonWidgetClass;
};
