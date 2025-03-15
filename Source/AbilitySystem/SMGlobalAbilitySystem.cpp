// Copyright Studio Surround. All Rights Reserved.


#include "SMGlobalAbilitySystem.h"

#include "GameplayAbilitySpec.h"
#include "SMAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

void FGlobalAppliedAbilityList::AddToASC(const TSubclassOf<UGameplayAbility>& Ability, USMAbilitySystemComponent* ASC)
{
	if (Handles.Contains(ASC))
	{
		RemoveFromASC(ASC);
	}

	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(USMAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->ClearAbility(KVP.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffectList::AddToASC(const TSubclassOf<UGameplayEffect>& Effect, USMAbilitySystemComponent* ASC)
{
	if (Handles.Contains(ASC))
	{
		RemoveFromASC(ASC);
	}

	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, /*Level=*/ 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(USMAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->RemoveActiveGameplayEffect(KVP.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedTagList::AddToASC(FGameplayTag Tag, USMAbilitySystemComponent* ASC)
{
	if (Tags.Contains(ASC))
	{
		RemoveFromASC(ASC);
	}

	ASC->AddTag(Tag);
	Tags.Add(ASC, Tag);
}

void FGlobalAppliedTagList::RemoveFromASC(USMAbilitySystemComponent* ASC)
{
	if (FGameplayTag* Tag = Tags.Find(ASC))
	{
		ASC->RemoveTag(*Tag);
		Tags.Remove(ASC);
	}
}

void FGlobalAppliedTagList::RemoveFromAll()
{
	for (auto& KVP : Tags)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->RemoveTag(KVP.Value);
		}
	}
	Tags.Empty();
}

USMGlobalAbilitySystem::USMGlobalAbilitySystem()
{
}

USMGlobalAbilitySystem& USMGlobalAbilitySystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	USMGlobalAbilitySystem* GlobalAbilitySystem = World->GetSubsystem<USMGlobalAbilitySystem>();
	check(GlobalAbilitySystem);
	return *GlobalAbilitySystem;
}

void USMGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr) && (!AppliedAbilities.Contains(Ability)))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);
		for (USMAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Ability, ASC);
		}
	}
}

void USMGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr) && (!AppliedEffects.Contains(Effect)))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);
		for (USMAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Effect, ASC);
		}
	}
}

void USMGlobalAbilitySystem::ApplyTagToAll(FGameplayTag Tag)
{
	if (!AppliedTags.Contains(Tag))
	{
		FGlobalAppliedTagList& Entry = AppliedTags.Add(Tag);
		for (USMAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Tag, ASC);
		}
	}
}

void USMGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr) && AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
		Entry.RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void USMGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr) && AppliedEffects.Contains(Effect))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
		Entry.RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}

void USMGlobalAbilitySystem::RemoveTagFromAll(FGameplayTag Tag)
{
	if (AppliedTags.Contains(Tag))
	{
		FGlobalAppliedTagList& Entry = AppliedTags[Tag];
		Entry.RemoveFromAll();
		AppliedTags.Remove(Tag);
	}
}

void USMGlobalAbilitySystem::RegisterASC(USMAbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}
	for (auto& Entry : AppliedTags)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}

	RegisteredASCs.AddUnique(ASC);
}

void USMGlobalAbilitySystem::UnregisterASC(USMAbilitySystemComponent* ASC)
{
	check(ASC);
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (auto& Entry : AppliedTags)
	{
		Entry.Value.RemoveFromASC(ASC);
	}

	RegisteredASCs.Remove(ASC);
}
