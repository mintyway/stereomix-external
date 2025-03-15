// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMHIC_Character.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Utilities/SMLog.h"

USMGameplayAbility::USMGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

USMAbilitySystemComponent* USMGameplayAbility::GetASC() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return Cast<USMAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
}

AActor* USMGameplayAbility::GetAvatarActor() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return CurrentActorInfo->AvatarActor.Get();
}

ASMPlayerCharacterBase* USMGameplayAbility::GetCharacter() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return Cast<ASMPlayerCharacterBase>(CurrentActorInfo->AvatarActor.Get());
}

USMHIC_Character* USMGameplayAbility::GetHIC() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent<USMHIC_Character>(CurrentActorInfo->AvatarActor.Get());
}

const USMPlayerCharacterDataAsset* USMGameplayAbility::GetDataAsset() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(CurrentActorInfo->AvatarActor.Get());
	if (!SourceCharacter)
	{
		return nullptr;
	}

	return SourceCharacter->GetDataAsset();
}

bool USMGameplayAbility::TryGiveDamageToTarget(AActor* TargetActor, float DamageAmount) const
{
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(TargetActor);
	if (!TargetDamageInterface || TargetDamageInterface->CanIgnoreAttack())
	{
		return false;
	}

	const ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (USMTeamBlueprintLibrary::IsSameTeam(SourceCharacter, TargetActor))
	{
		return false;
	}

	TargetDamageInterface->ReceiveDamage(GetAvatarActor(), DamageAmount, AbilityTags);
	return true;
}

FGameplayCueParameters USMGameplayAbility::MakeGameplayCueParameters() const
{
	FGameplayCueParameters CueParameters;
	CueParameters.SourceObject = GetAvatarActor();
	CueParameters.Instigator = GetAvatarActor();
	CueParameters.AggregatedSourceTags = AbilityTags;
	return CueParameters;
}

FGameplayCueParameters USMGameplayAbility::MakeGameplayCueParameters(const FVector& Location, const FVector& Direction) const
{
	FGameplayCueParameters CueParameters = MakeGameplayCueParameters();
	CueParameters.Location = Location;
	CueParameters.Normal = Direction;
	return CueParameters;
}

FGameplayCueParameters USMGameplayAbility::MakeGameplayCueParametersAttached(UObject* TargetToAttach, const FVector& Offset, const FVector& Direction) const
{
	FGameplayCueParameters CueParameters = MakeGameplayCueParameters(Offset, Direction);
	if (TargetToAttach)
	{
		if (AActor* TargetActor = Cast<AActor>(TargetToAttach))
		{
			CueParameters.TargetAttachComponent = TargetActor->GetRootComponent();
		}
		else if (USceneComponent* TargetComponent = Cast<USceneComponent>(TargetToAttach))
		{
			CueParameters.TargetAttachComponent = TargetComponent;
		}
	}
	return CueParameters;
}

void USMGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USMGameplayAbility::EndAbilityByCancel()
{
	check(CurrentActorInfo);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
