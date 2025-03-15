// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActorInitStateInterface.h"
#include "CommonPlayerController.h"
#include "GameplayTagContainer.h"
#include "LoadingProcessInterface.h"
#include "Data/SMTeam.h"
#include "Data/SMControlData.h"
#include "Input/InputMappingTypes.h"
#include "Interfaces/SMCharacterInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "UI/Game/SMGameHUDLayout.h"

#include "SMPlayerController.generated.h"

class ILevelInstanceInterface;
class ASMPlayerController;
class USMTransitionScreen;
class USMControlData;
class USMCharacterDataAsset;
class UInputMappingContext;
class ASMPlayerCharacterBase;
class UCommonActivatableWidget;
class USMPlayerControllerDataAsset;
class USMGameHUDLayout;
enum class ESMCharacterType : uint8;


UCLASS(Abstract, DisplayName = "StereoMix Player Controller")
class STEREOMIX_API ASMPlayerController : public ACommonPlayerController, public IAbilitySystemInterface, public IActorInitStateInterface, public ISMTeamInterface, public ISMCharacterInterface, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	ASMPlayerController();
	virtual void PostInitializeComponents() override;
	virtual void InitPlayerState() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void AddCheats(bool bForce) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual bool ShouldShowLoadingScreen(FString& OutReason) override;

	UFUNCTION(BlueprintPure, Category = "StereoMix|PlayerController")
	virtual bool IsReadyToPlay() const override { return bIsReadyToPlay; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|PlayerController")
	virtual ESMTeam GetTeam() const override;

	virtual USMTeamComponent* GetTeamComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|PlayerController")
	virtual ESMCharacterType GetCharacterType() const override;

	virtual void SetCharacterType(ESMCharacterType CharacterType) override;

	virtual FSMOnCharacterChanged* GetOnCharacterChangedDelegate() override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|PlayerController")
	USMPrimaryLayout* GetPrimaryWidget() const { return PrimaryWidget; }

	USMPrimaryLayout* InitializeUserInterface();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "StereoMix|PlayerController")
	void RequestImmediateResetPosition();

	TSoftClassPtr<USMTransitionScreen> GetTransitionScreenClass() const { return TransitionScreenClass; }

	virtual FOnActorReadyToPlay& GetOnReadyToPlayEvent() override { return OnReadyToPlayEvent; }

	virtual FOnActorReadyToPlayDynamic& GetOnReadyToPlayDynamicEvent() override { return K2_OnReadyToPlayEvent; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void PostSeamlessTravel() override;

	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;

	void BindPlayerTeamChangedEvent();
	void BindPlayerCharacterTypeChangedEvent();

	virtual void SetReadyToPlay(bool bReadyToPlay) override;
	void CheckIsReadyToPlay();

	UFUNCTION(Server, Reliable)
	void ServerReadyToPlay();

	virtual void OnLevelLoaded();
	void OnPlayerTeamChanged(AActor* ObjectChangingCharacter, ESMTeam PreviousTeam, ESMTeam NewTeam);
	void OnPlayerCharacterTypeChanged(UObject* ObjectChangingCharacter, ESMCharacterType PreviousCharacterType, ESMCharacterType NewCharacterType);
	void OnControllerReadyToPlay(IActorInitStateInterface* ActorToReady);

	FOnActorReadyToPlay OnReadyToPlayEvent;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|PlayerController", meta = (DisplayName = "On Ready To Play"))
	FOnActorReadyToPlayDynamic K2_OnReadyToPlayEvent;

	/**
	 * 클라이언트의 초기화 완료 시점에 폰 여부를 포함할 지 여부를 정합니다.
	 *
	 * 만약 월드에서 폰을 사용하지 않는데 해당 값이 true라면 플레이어컨트롤러는 무한 로딩 상태가 되므로 주의해야 합니다.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|PlayerController")
	bool bShouldInitializePawn = false;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|UI", DisplayName = "Primary Layout")
	TSoftClassPtr<USMPrimaryLayout> PrimaryLayoutClass;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|UI", DisplayName = "Layer", meta=(Categories = "UI.Layer"))
	FGameplayTag LayerTag;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|UI")
	bool bAutoInitializeUserInterface = true;

	/** 트랜지션 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|UI|Transition")
	TSoftClassPtr<USMTransitionScreen> TransitionScreenClass;

	/** 월드 시작 시 자동으로 트랜지션을 실행할지 여부를 정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|UI|Transition")
	bool bAutoTransitionOnBeginPlay = true;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Input")
	TArray<FSMInputMappingContextSetting> InputMappingContextSettings;

private:
	UPROPERTY()
	TObjectPtr<USMPrimaryLayout> PrimaryWidget;

	uint32 bIsReadyToPlay : 1 = false;
	uint32 bIsUserInterfaceInitialized : 1 = false;

	double LastLoadingScreenCheckTime = 0.0;
	bool bPlayerInitializationDelayInProgress = false;

	FDelegateHandle PlayerTeamChangedEventHandle;
	FDelegateHandle PlayerCharacterTypeChangedEventHandle;
};
