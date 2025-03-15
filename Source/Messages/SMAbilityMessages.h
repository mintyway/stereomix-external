// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "SMAbilityMessages.generated.h"

class USMBuffDataAsset;

USTRUCT(BlueprintType)
struct FSMGameplayAttributeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	TObjectPtr<const AActor> Target;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay|Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay|Attribute")
	FGameplayAttributeData OldValue;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay|Attribute")
	FGameplayAttributeData NewValue;
};

USTRUCT(BlueprintType, DisplayName = "StereoMix Gameplay Ability Slot Message")
struct FSMGameplayAbilitySlotMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	int32 InputID = -1;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	FGameplayAbilitySpecHandle AbilitySpecHandle;
};

USTRUCT(BlueprintType)
struct FSMGameplayTagMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	TObjectPtr<AActor> Target;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	FGameplayTag UpdatedTag;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	uint8 bAdded : 1 = false;
};

USTRUCT(BlueprintType)
struct FSMGameplayCueMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	TObjectPtr<AActor> Target;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	FGameplayTag GameplayCueTag;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	TEnumAsByte<EGameplayCueEvent::Type> EventType{};

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	FGameplayCueParameters CueParameters;
};

USTRUCT(BlueprintType)
struct FSMBuffMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	TObjectPtr<AActor> Target;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	FGameplayTag BuffTag;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	TObjectPtr<USMBuffDataAsset> Data;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	uint8 bAdded : 1 = false;

	UPROPERTY(BlueprintReadOnly, Category = "StereoMix|Gameplay")
	float Duration = 0.0f;
};
