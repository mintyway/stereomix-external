// Copyright Studio Surround. All Rights Reserved.


#include "SMAbilitySystemComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "EngineUtils.h"
#include "SMGlobalAbilitySystem.h"
#include "Abilities/SMGameplayAbility.h"
#include "AttributeSets/SMCharacterAttributeSet.h"
#include "GameplayEffect/SMGameplayEffect_DynamicTag.h"
#include "GameplayCueManager.h"
#include "StereoMixLog.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "Utilities/SMLog.h"


USMAbilitySystemComponent::USMAbilitySystemComponent()
{
}

void USMAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USMAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (USMGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<USMGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}

	Super::EndPlay(EndPlayReason);
}

void USMAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
	if (USMGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<USMGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->RegisterASC(this);
	}
}

void USMAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		USMGameplayAbility* StereoMixAbilityCDO = Cast<USMGameplayAbility>(AbilitySpec.Ability);
		if (!StereoMixAbilityCDO)
		{
			UE_LOG(LogStereoMixAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Non-SMGameplayAbility %s was Granted to ASC. Skipping."), *AbilitySpec.Ability.GetName());
			continue;
		}

		if (StereoMixAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// Cancel all the spawned instances, not the CDO.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				USMGameplayAbility* StereoMixAbilityInstance = CastChecked<USMGameplayAbility>(AbilityInstance);

				if (ShouldCancelFunc(StereoMixAbilityInstance, AbilitySpec.Handle))
				{
					if (StereoMixAbilityInstance->CanBeCanceled())
					{
						StereoMixAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), StereoMixAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(LogStereoMixAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *StereoMixAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// Cancel the non-instanced ability CDO.
			if (ShouldCancelFunc(StereoMixAbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled.
				check(StereoMixAbilityCDO->CanBeCanceled());
				StereoMixAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void USMAbilitySystemComponent::AddTag(const FGameplayTag& InGameplayTag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = USMGameplayEffect_DynamicTag::StaticClass();
	if (!ensure(DynamicTagGE))
	{
		return;
	}

	const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
	if (!ensure(GESpec))
	{
		return;
	}

	GESpec->DynamicGrantedTags.AddTag(InGameplayTag);
	ApplyGameplayEffectSpecToSelf(*GESpec);
}

void USMAbilitySystemComponent::RemoveTag(const FGameplayTag& InGameplayTag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = USMGameplayEffect_DynamicTag::StaticClass();
	if (!ensure(DynamicTagGE))
	{
		return;
	}

	FGameplayEffectQuery GEQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(InGameplayTag));
	GEQuery.EffectDefinition = DynamicTagGE;

	RemoveActiveEffects(GEQuery);
}

void USMAbilitySystemComponent::AddGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		ServerRequestGCReliable(TargetActor, GameplayCueTag, EGameplayCueEvent::OnActive, Parameters, false);
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		ServerRequestGCReliable(TargetActor, GameplayCueTag, EGameplayCueEvent::OnActive, Parameters, true);
		ClientExecuteGC(TargetActor, GameplayCueTag, EGameplayCueEvent::OnActive, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

void USMAbilitySystemComponent::RemoveGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		ServerRequestGCReliable(TargetActor, GameplayCueTag, EGameplayCueEvent::Removed, Parameters, false);
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		ServerRequestGCReliable(TargetActor, GameplayCueTag, EGameplayCueEvent::Removed, Parameters, true);
		ClientExecuteGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Removed, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

void USMAbilitySystemComponent::ExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters, bool bShouldReliable) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		if (bShouldReliable)
		{
			ServerRequestGCReliable(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters, false);
		}
		else
		{
			ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters, false);
		}
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		if (bShouldReliable)
		{
			ServerRequestGCReliable(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters, true);
		}
		else
		{
			ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters, true);
		}
		ClientExecuteGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

int32 USMAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	ClientReceiveEvent(EventTag, *Payload);
	return Super::HandleGameplayEvent(EventTag, Payload);
}

USMGameplayAbility* USMAbilitySystemComponent::K2_GetGAInstanceFromClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const
{
	return Cast<USMGameplayAbility>(InternalGetGAInstanceFromClass(InAbilityClass));
}

void USMAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	// StateTag의 경우
	// Authority, AutonomousProxy -> 호출 O
	// SimulatedProxy -> 호출 X
	OnTagUpdatedEvent.Broadcast(Tag, TagExists);
	K2_OnGameplayTagUpdatedEvent.Broadcast(Tag, TagExists);
}

void USMAbilitySystemComponent::ClientReceiveEvent_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	OnGameplayEventReceivedEvent.Broadcast(EventTag, Payload);
	K2_OnGameplayEventReceivedEvent.Broadcast(EventTag, Payload);
}

void USMAbilitySystemComponent::ServerRequestGC_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf) const
{
	HandleServerRequestedGC(TargetActor, GameplayCueTag, CueEvent, Parameters, bExcludeSelf, false);
}

void USMAbilitySystemComponent::ServerRequestGCReliable_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf) const
{
	HandleServerRequestedGC(TargetActor, GameplayCueTag, CueEvent, Parameters, bExcludeSelf, true);
}

void USMAbilitySystemComponent::HandleServerRequestedGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf, bool bShouldReliable) const
{
#if WITH_SERVER_CODE
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	for (const APlayerController* const PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (bExcludeSelf && (PC == SourceCharacter->GetController()))
		{
			continue;
		}

		ACharacter* PendingCharacter = PC->GetCharacter();
		const USMAbilitySystemComponent* PendingASC = Cast<USMAbilitySystemComponent>(USMAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PendingCharacter));
		if (!PendingASC)
		{
			continue;
		}

		if (bShouldReliable)
		{
			PendingASC->ClientExecuteGCReliable(TargetActor, GameplayCueTag, CueEvent, Parameters);
		}
		else
		{
			PendingASC->ClientExecuteGC(TargetActor, GameplayCueTag, CueEvent, Parameters);
		}
	}
#endif
}

void USMAbilitySystemComponent::ClientExecuteGC_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters) const
{
	Parameters.OriginalTag = GameplayCueTag;

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(USMAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	switch (CueEvent)
	{
		case EGameplayCueEvent::OnActive:
		{
			UGameplayCueManager::AddGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
			// Target에 대한 게임플레이큐 이벤트이기 때문에 타겟의 ASC에서 호출해야 함
			if (TargetASC)
			{
				TargetASC->OnGameplayCueAddEvent.Broadcast(GameplayCueTag, Parameters);
				TargetASC->K2_OnGameplayCueAddEvent.Broadcast(GameplayCueTag, Parameters);
			}
			break;
		}
		case EGameplayCueEvent::WhileActive:
		{
			break;
		}
		case EGameplayCueEvent::Executed:
		{
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
			if (TargetASC)
			{
				TargetASC->OnGameplayCueExecuteEvent.Broadcast(GameplayCueTag, Parameters);
				TargetASC->K2_OnGameplayCueExecuteEvent.Broadcast(GameplayCueTag, Parameters);
			}
			break;
		}
		case EGameplayCueEvent::Removed:
		{
			UGameplayCueManager::RemoveGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
			if (TargetASC)
			{
				TargetASC->OnGameplayCueRemoveEvent.Broadcast(GameplayCueTag, Parameters);
				TargetASC->K2_OnGameplayCueRemoveEvent.Broadcast(GameplayCueTag, Parameters);
			}
			break;
		}
	}
}

void USMAbilitySystemComponent::ClientExecuteGCReliable_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters) const
{
	ClientExecuteGC_Implementation(TargetActor, GameplayCueTag, CueEvent, Parameters);
}

UGameplayAbility* USMAbilitySystemComponent::InternalGetGAInstanceFromClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const
{
	for (const FGameplayAbilitySpec& GASpec : GetActivatableAbilities())
	{
		if (GASpec.Ability && GASpec.Ability.IsA(InAbilityClass))
		{
			for (UGameplayAbility* AbilityInstance : GASpec.GetAbilityInstances())
			{
				return AbilityInstance;
			}
		}
	}

	return nullptr;
}
