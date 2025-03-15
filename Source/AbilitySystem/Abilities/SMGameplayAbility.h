// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "SMGameplayAbility.generated.h"

class ASMPlayerCharacterBase;
class USMAbilitySystemComponent;
class USMPlayerCharacterDataAsset;
class USMHIC_Character;

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName = "StereoMix Gameplay Ability")
class STEREOMIX_API USMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility();

	USMAbilitySystemComponent* GetASC() const;

	template<typename T>
	T* GetASC() const { return Cast<T>(GetASC()); }

	AActor* GetAvatarActor() const;

	template<typename T>
	T* GetAvatarActor() const { return Cast<T>(GetAvatarActor()); }

	ASMPlayerCharacterBase* GetCharacter() const;

	template<typename T>
	T* GetCharacter() const { return Cast<T>(GetCharacter()); }

	USMHIC_Character* GetHIC() const;

	template<typename T>
	T* GetHIC() const { return Cast<T>(GetHIC()); }

	const USMPlayerCharacterDataAsset* GetDataAsset() const;

	template<typename T>
	const T* GetDataAsset() const { return Cast<T>(GetDataAsset()); }

	UFUNCTION()
	bool TryGiveDamageToTarget(AActor* TargetActor, float DamageAmount) const;

	/** 어빌리티 태그가 포함된 게임플레이 큐 파라미터를 생성합니다. */
	FGameplayCueParameters MakeGameplayCueParameters() const;

	/** 어빌리티 태그, 위치, 방향이 포함된 게임플레이 큐 파라미터를 생성합니다. */
	FGameplayCueParameters MakeGameplayCueParameters(const FVector& Location, const FVector& Direction) const;

	FGameplayCueParameters MakeGameplayCueParametersAttached(UObject* TargetToAttach, const FVector& Offset, const FVector& Direction) const;

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UFUNCTION()
	void EndAbilityByCancel();
};
