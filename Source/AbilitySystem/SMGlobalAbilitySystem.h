// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "SMGlobalAbilitySystem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class USMAbilitySystemComponent;
class UObject;
struct FActiveGameplayEffectHandle;
struct FFrame;
struct FGameplayAbilitySpecHandle;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<USMAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayAbility>& Ability, USMAbilitySystemComponent* ASC);
	void RemoveFromASC(USMAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<USMAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayEffect>& Effect, USMAbilitySystemComponent* ASC);
	void RemoveFromASC(USMAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedTagList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<USMAbilitySystemComponent>, FGameplayTag> Tags;

	void AddToASC(FGameplayTag Tag, USMAbilitySystemComponent* ASC);
	void RemoveFromASC(USMAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

class USMAbilitySystemComponent;
/**
 * 
 */
UCLASS(DisplayName = "StereoMix Global Ability System")
class STEREOMIX_API USMGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	USMGlobalAbilitySystem();

	static USMGlobalAbilitySystem& Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="StereoMix")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="StereoMix")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="StereoMix")
	void ApplyTagToAll(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "StereoMix")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "StereoMix")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "StereoMix")
	void RemoveTagFromAll(FGameplayTag Tag);

	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(USMAbilitySystemComponent* ASC);

	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(USMAbilitySystemComponent* ASC);

private:
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TMap<FGameplayTag, FGlobalAppliedTagList> AppliedTags;

	UPROPERTY()
	TArray<TObjectPtr<USMAbilitySystemComponent>> RegisteredASCs;
};
