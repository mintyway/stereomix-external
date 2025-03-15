// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_Transition.generated.h"

struct FGameplayTag;
enum class ESMTransitionType : uint8;
class USMTransitionScreen;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSMTransitionCompleteDelegate);


UCLASS()
class STEREOMIX_API UAsyncAction_Transition : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** 로컬 플레이어 컨트롤러의 SMTransitionComponent를 참고하여 트랜지션을 재생합니다. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(BlueprintInternalUseOnly, WorldContext="InWorldContextObject", Categories="Transition"))
	static UAsyncAction_Transition* TransIn(UObject* InWorldContextObject);

	/** 로컬 플레이어 컨트롤러의 SMTransitionComponent를 참고하여 트랜지션을 재생합니다. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(BlueprintInternalUseOnly, WorldContext="InWorldContextObject", Categories="Transition"))
	static UAsyncAction_Transition* TransOut(UObject* InWorldContextObject);
	
	/**
	 * 트랜지션 인 애니메이션을 재생합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(BlueprintInternalUseOnly, WorldContext="InWorldContextObject", Categories="Transition"))
	static UAsyncAction_Transition* TransInUsingSpecificTransition(UObject* InWorldContextObject, TSoftClassPtr<USMTransitionScreen> InTransitionScreenClass);
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(BlueprintInternalUseOnly, WorldContext="InWorldContextObject", Categories="Transition"))
	static UAsyncAction_Transition* TransOutUsingSpecificTransition(UObject* InWorldContextObject, TSoftClassPtr<USMTransitionScreen> InTransitionScreenClass);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, DisplayName="Completed")
	FSMTransitionCompleteDelegate OnCompleted;

private:
	UFUNCTION()
	void OnTransitionCompleted();

	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY(Transient)
	TSoftClassPtr<USMTransitionScreen> TransitionScreenClass;

	UPROPERTY(Transient)
	ESMTransitionType TransitionType;
};
