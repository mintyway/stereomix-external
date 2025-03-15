// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "SMAbilitySystemComponent.generated.h"

class USMGameplayAbility;

DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayEventReceived, const FGameplayTag& EventTag, const FGameplayEventData& Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayEventReceivedDynamic, const FGameplayTag&, EventTag, const FGameplayEventData&, Payload);

DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayTagUpdated, const FGameplayTag& Tag, bool TagExists);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayTagUpdatedDynamic, const FGameplayTag&, Tag, bool, TagExists);

DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayCueAdd, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayCueAddDynamic, const FGameplayTag&, GameplayCueTag, const FGameplayCueParameters&, Parameters);

DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayCueRemove, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayCueRemoveDynamic, const FGameplayTag&, GameplayCueTag, const FGameplayCueParameters&, Parameters);

DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayCueExecute, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSMOnGameplayCueExecuteDynamic, const FGameplayTag&, GameplayCueTag, const FGameplayCueParameters&, Parameters);

UCLASS(DisplayName = "StereoMix Ability System Component", meta = (BlueprintSpawnableComponent))
class STEREOMIX_API USMAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	USMAbilitySystemComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	typedef TFunctionRef<bool(const USMGameplayAbility* StereoMixAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

	/** 태그를 추가하고 리플리케이트 합니다. 서버에서만 실행되어야합니다. */
	void AddTag(const FGameplayTag& InGameplayTag);

	/** 태그를 제거하고 리플리케이트 합니다. 서버에서만 실행되어야합니다. */
	void RemoveTag(const FGameplayTag& InGameplayTag);

	/** 게임플레이 큐를 추가하고 전파합니다.
	 * RawMagnitude 등으로 예상되는 지속시간을 같이 넘겨 로컬에서 예기치 못하게 RemoveGC가 호출되지 못했을 때 이펙트가 지속되는 것을 방지하는 것을 권장합니다.
	 */
	void AddGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const;

	/** 게임플레이 큐를 제거하고 전파합니다. */
	void RemoveGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const;

	/** 게임플레이 큐를 실행하고 전파합니다. 1회성 이펙트에 대해서만 써야 합니다. */
	void ExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters, bool bShouldReliable = false) const;

	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;

	/** 자신에게 부여된 어빌리티를 클래스를 기준으로 찾습니다. 기존 함수는 정확히 일치하는 클래스만 찾지만, 이 함수는 해당 클래스를 상속한 서브 클래스도 찾습니다. */
	template<typename T>
	T* GetGAInstanceFromClass() const { return Cast<T>(InternalGetGAInstanceFromClass(T::StaticClass())); }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|AbilitySystem", DisplayName = "Get GA From Class")
	USMGameplayAbility* K2_GetGAInstanceFromClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const;

	/** 게임플레이 이벤트가 발생했을 때 발생합니다. */
	FSMOnGameplayEventReceived OnGameplayEventReceivedEvent;

	/** 게임플레이 이벤트가 발생했을 때 발생합니다. */
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|AbilitySystem", DisplayName = "On Gameplay Event Received")
	FSMOnGameplayEventReceivedDynamic K2_OnGameplayEventReceivedEvent;

	/** 어빌리티시스템의 태그가 추가되거나 없어졌을 때 발생합니다. */
	FSMOnGameplayTagUpdated OnTagUpdatedEvent;

	/** 어빌리티시스템의 태그가 추가되거나 없어졌을 때 발생합니다. */
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|AbilitySystem", DisplayName = "On Tag Updated")
	FSMOnGameplayTagUpdatedDynamic K2_OnGameplayTagUpdatedEvent;

	/** 어빌리티시스템에 게임플레이 큐가 추가되었을 때 발생합니다. */
	FSMOnGameplayCueAdd OnGameplayCueAddEvent;

	/** 어빌리티시스템에 게임플레이 큐가 추가되었을 때 발생합니다. */
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|AbilitySystem", DisplayName = "On Gameplay Cue Add")
	FSMOnGameplayCueAddDynamic K2_OnGameplayCueAddEvent;

	/** 어빌리티시스템에 게임플레이 큐가 제거되었을 때 발생합니다. */
	FSMOnGameplayCueRemove OnGameplayCueRemoveEvent;

	/** 어빌리티시스템에 게임플레이 큐가 제거되었을 때 발생합니다. */
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|AbilitySystem", DisplayName = "On Gameplay Cue Remove")
	FSMOnGameplayCueRemoveDynamic K2_OnGameplayCueRemoveEvent;

	/** 어빌리티시스템에 게임플레이 큐가 실행되었을 때 발생합니다. */
	FSMOnGameplayCueExecute OnGameplayCueExecuteEvent;

	/** 어빌리티시스템에 게임플레이 큐가 실행되었을 때 발생합니다. */
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|AbilitySystem", DisplayName = "On Gameplay Cue Execute")
	FSMOnGameplayCueExecuteDynamic K2_OnGameplayCueExecuteEvent;

protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

	/**
	 * 클라이언트로 이벤트를 전파합니다.
	 * 어디까지나 이런 이벤트가 발생했었다 라는 것을 전달하는 것이며, 클라이언트에서는 이벤트를 처리하지 않습니다. 따라서 비신뢰성으로 전달합니다.
	 */
	UFUNCTION(Client, Unreliable)
	void ClientReceiveEvent(FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(Server, Unreliable)
	void ServerRequestGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf) const;

	UFUNCTION(Server, Reliable)
	void ServerRequestGCReliable(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf) const;

	UFUNCTION()
	virtual void HandleServerRequestedGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf, bool bShouldReliable) const;

	UFUNCTION(Client, Unreliable)
	void ClientExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters) const;

	UFUNCTION(Client, Reliable)
	void ClientExecuteGCReliable(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters) const;

	UGameplayAbility* InternalGetGAInstanceFromClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const;
};
