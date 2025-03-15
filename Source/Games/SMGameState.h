// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "LoadingProcessInterface.h"
#include "ModularGameState.h"

#include "SMGameState.generated.h"

enum class ESMMatchServerType : uint8;
class USMMatchDefinition;
class ASMPlayerState;
class USMAbilitySystemComponent;
class USMDesignData;
struct FSMVerbMessage;

DECLARE_MULTICAST_DELEGATE_OneParam(FSMOnPlayerAdded, ASMPlayerState*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnPlayerAddedDynamic, ASMPlayerState*, PlayerState);

DECLARE_MULTICAST_DELEGATE_OneParam(FSMOnPlayerRemoved, ASMPlayerState*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnPlayerRemovedDynamic, ASMPlayerState*, PlayerState);

DECLARE_MULTICAST_DELEGATE(FSMOnGameReadyToStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSMOnGameReadyToStartDynamic);

UCLASS()
class STEREOMIX_API ASMGameState : public AModularGameStateBase, public IAbilitySystemInterface, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	ASMGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual bool ShouldShowLoadingScreen(FString& OutReason) override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|GameState")
	USMAbilitySystemComponent* GetStereoMixAbilitySystemComponent() const;

	/**
	 * 모든 클라이언트에게 메시지를 전달합니다. Unreliable하므로 메시지 전달을 보장받지 못합니다.
	 * @param Message 전달할 메시지
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "StereoMix|GameState")
	void MulticastMessage(const FSMVerbMessage Message);

	/**
	 * 모든 클라이언트에게 메시지를 전달합니다. Reliable하기 때문에 반드시 전달해야 하는 메시지가 있을 때 사용합니다.
	 * @param Message 전달할 메시지
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "StereoMix|GameState")
	void MulticastReliableMessage(const FSMVerbMessage Message);

	/** 서버의 FPS를 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "StereoMix|GameState")
	float GetServerFPS() const { return ServerFPS; }

	UFUNCTION(BlueprintPure, Category = "StereoMix|GameState")
	bool IsReadyToStart() const { return bIsReadyToStart; }

	/**
	 * 게임이 시작할 때 호출되는 델리게이트를 등록합니다. 이미 게임이 시작되었다면 즉시 델리게이트를 실행합니다.
	 * @param Delegate 등록할 델리게이트
	 */
	void StartListeningGameReadyToStart(FSMOnGameReadyToStart::FDelegate&& Delegate);

	FSMOnPlayerAdded OnPlayerAddedEvent;
	FSMOnPlayerRemoved OnPlayerRemovedEvent;
	FSMOnGameReadyToStart OnGameReadyToStartEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|GameState", meta = (DisplayName = "On Player Added"))
	FSMOnPlayerAddedDynamic K2_OnPlayerAddedEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|GameState", meta = (DisplayName = "On Player Removed"))
	FSMOnPlayerRemovedDynamic K2_OnPlayerRemovedEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|GameState", meta = (DisplayName = "On Game Ready To Start"))
	FSMOnGameReadyToStartDynamic K2_OnGameReadyToStartEvent;

protected:
	virtual void Tick(float DeltaSeconds) override;

	void SetReadyToPlay();

	UPROPERTY(Replicated)
	float ServerFPS = 0.f;

private:
	UFUNCTION()
	void OnRep_IsReadyToStart() const;

	UPROPERTY(VisibleAnywhere, Category = "StereoMix|GameState")
	TObjectPtr<USMAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing = OnRep_IsReadyToStart)
	uint8 bIsReadyToStart : 1 = false;

	friend class ASMGameMode;
};
