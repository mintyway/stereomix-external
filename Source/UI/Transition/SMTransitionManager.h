// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoadingScreenManager.h"
#include "SMTransitionManager.generated.h"

class IActorInitStateInterface;
struct FGameplayTag;
class USMTransitionScreen;

UENUM(BlueprintType, DisplayName = "Transition Type")
enum class ESMTransitionType : uint8
{
	// Game -> LoadingScreen
	In,

	// LoadingScreen -> Game
	Out
};

DECLARE_DELEGATE(FSMTransitionDelegate);

DECLARE_MULTICAST_DELEGATE_OneParam(FSMOnTransitionCompleted, ESMTransitionType TransitionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnTransitionCompletedDynamic, ESMTransitionType, TransitionType);

USTRUCT()
struct FSMTransitionProcess
{
	GENERATED_BODY()

	FSMTransitionProcess()
		: bIsSet(false), TransitionType(ESMTransitionType::In)
	{
	}

	FSMTransitionProcess(ESMTransitionType InTransitionType, const FSMTransitionDelegate& InCompletedDelegate)
		: bIsSet(true), TransitionType(InTransitionType), CompletedDelegate(InCompletedDelegate)
	{
	}

	bool IsSet() const { return bIsSet; }
	ESMTransitionType GetTransitionType() const { return TransitionType; }
	const FSMTransitionDelegate& GetCompletedDelegate() const { return CompletedDelegate; }

	void Reset()
	{
		bIsSet = false;
		TransitionType = ESMTransitionType::In;
		CompletedDelegate.Unbind();
	}

private:
	uint8 bIsSet : 1;
	ESMTransitionType TransitionType;
	FSMTransitionDelegate CompletedDelegate;
};


UCLASS(DisplayName = "StereoMix Transition Manager")
class STEREOMIX_API USMTransitionManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static USMTransitionManager* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Transition", meta=(BlueprintInternalUseOnly, WorldContext="InWorldContextObject"))
	static bool IsTransOut(const UObject* InWorldContextObject);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	bool PrepareTransitionScreen(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass);

	bool IsTransitioning() const { return ActiveTransition.IsSet(); }

	template<class UserClass>
	void DoTransition(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass, ESMTransitionType TransitionType, UserClass* InWorldContextObject, FSMTransitionDelegate::TMethodPtr<UserClass> OnCompleted)
	{
		DoTransitionInternal(TransitionScreenClass, TransitionType, FSMTransitionDelegate::CreateUObject(InWorldContextObject, OnCompleted));
	}

	template<class UserClass>
	void DoTransition(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass, ESMTransitionType TransitionType, UserClass* InWorldContextObject, FSMTransitionDelegate::TConstMethodPtr<UserClass> OnCompleted)
	{
		DoTransitionInternal(TransitionScreenClass, TransitionType, FSMTransitionDelegate::CreateUObject(InWorldContextObject, OnCompleted));
	}

	void DoTransition(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass, ESMTransitionType TransitionType, TFunctionRef<void()> OnCompleted)
	{
		DoTransitionInternal(TransitionScreenClass, TransitionType, FSMTransitionDelegate::CreateLambda(OnCompleted));
	}

	void DoTransition(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass, ESMTransitionType TransitionType)
	{
		DoTransition(TransitionScreenClass, TransitionType, [] {
		});
	}

	template<class UserClass>
	void DoTransition(ESMTransitionType TransitionType, UserClass* InWorldContextObject, FSMTransitionDelegate::TMethodPtr<UserClass> OnCompleted)
	{
		DoTransitionInternal(nullptr, TransitionType, FSMTransitionDelegate::CreateUObject(InWorldContextObject, OnCompleted));
	}

	template<class UserClass>
	void DoTransition(ESMTransitionType TransitionType, UserClass* InWorldContextObject, FSMTransitionDelegate::TConstMethodPtr<UserClass> OnCompleted)
	{
		DoTransitionInternal(nullptr, TransitionType, FSMTransitionDelegate::CreateUObject(InWorldContextObject, OnCompleted));
	}

	void DoTransition(ESMTransitionType TransitionType, TFunctionRef<void()> OnCompleted)
	{
		DoTransitionInternal(nullptr, TransitionType, FSMTransitionDelegate::CreateLambda(OnCompleted));
	}

	void DoTransition(ESMTransitionType TransitionType)
	{
		DoTransition(nullptr, TransitionType, [] {
		});
	}

	FSMOnTransitionCompleted OnTransitionCompletedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Transition", DisplayName = "On Transition Completed")
	FSMOnTransitionCompletedDynamic K2_OnTransitionCompletedEvent;

private:
	void DoTransitionInternal(const TSoftClassPtr<USMTransitionScreen>& TransitionScreenClass, ESMTransitionType TransitionType, FSMTransitionDelegate&& InCompletedDelegate);

	void HandleLocalPlayerReadyToPlay(IActorInitStateInterface* ActorToReady);

	void StartBlockingInput();
	void StopBlockingInput();

	UFUNCTION()
	void OnLoadingScreenVisibilityChanged(bool bVisibleLoadingScreen);

	void StartTransition(bool bForceStart = false);

	UFUNCTION()
	void HandleTransitionCompleted();

	UPROPERTY(Transient)
	TObjectPtr<ULoadingScreenManager> LoadingScreenManager;

	UPROPERTY(Transient)
	TObjectPtr<USMTransitionScreen> ActiveTransitionScreen;

	int32 TransitionScreenZOrder = 0;

	FSMTransitionProcess ActiveTransition;

	/** Input processor to eat all input while the loading screen is shown */
	TSharedPtr<IInputProcessor> InputPreProcessor;
};
