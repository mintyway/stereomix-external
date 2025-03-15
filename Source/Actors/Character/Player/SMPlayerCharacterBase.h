// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMTeamInterface.h"
#include "Data/Ability/SMActiveAbility.h"
#include "Data/SMCharacterType.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SMPlayerCharacterBase.generated.h"

class USMEmoticonWidgetComponent;
class USMCharacterLevelTransitionVisualComponent;
class USMCharacterLandingComponent;
class USMNeutralizeTimerComponent;
class USMFaceControlComponent;
class UTimelineComponent;
class ASMPlayerCharacterBase;
class ASMNoteBase;
class ASMWeaponBase;
class USMHIC_Character;
class ASMPlayerController;
class USMAbilitySystemComponent;
class USMPlayerCharacterDataAsset;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UNiagaraComponent;
class USMNameplateWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterLandedSignature, ASMPlayerCharacterBase* /*LandedCharacter*/);

UENUM(BlueprintType)
enum class EMoveTrailState : uint8
{
	Default,
	Catch,
	Immune
};

USTRUCT()
struct FSMCharacterStateEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag StateTag;

	UPROPERTY()
	double StartTime = 0.0;

	UPROPERTY()
	float DesiredDuration = -1.0f;

	void PreReplicatedRemove(const FFastArraySerializer& InArraySerializer);
	void PostReplicatedAdd(const FFastArraySerializer& InArraySerializer);
	void PostReplicatedChange(const FFastArraySerializer& InArraySerializer);
};

USTRUCT()
struct FSMCharacterStateArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSMCharacterStateEntry> States;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FSMCharacterStateEntry>(States, DeltaParams, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FSMCharacterStateArray> : TStructOpsTypeTraitsBase2<FSMCharacterStateArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS(Abstract)
class STEREOMIX_API ASMPlayerCharacterBase : public ASMCharacterBase, public IAbilitySystemInterface, public ISMTeamInterface, public ISMHoldInteractionInterface, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template<typename T>
	T* GetAbilitySystemComponent() const { return Cast<T>(GetAbilitySystemComponent()); }

	UFUNCTION(BlueprintPure, Category = "GAS")
	USMAbilitySystemComponent* GetStereoMixAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|Character")
	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; };

	UFUNCTION(BlueprintPure, Category = "StereoMix|Character")
	virtual ESMTeam GetTeam() const override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Character")
	const USMPlayerCharacterDataAsset* GetDataAsset() const { return DataAsset; }

	template<typename T>
	const T* GetDataAsset() const { return Cast<T>(DataAsset); }

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;

	template<typename T>
	T* GetHoldInteractionComponent() const { return Cast<T>(HIC); }

	/** 현재 커서가 위치한 곳의 좌표를 반환합니다. 양수 값의 사거리를 매개변수로 넘겨주면 사거리 내의 좌표를 반환합니다.
	 * bIsZeroBasis가 true면 캐릭터의 바닥을 기준으로, false면 캐릭터의 중심을 기준으로 계산합니다.
	 */
	FVector GetLocationFromCursor(bool bUseZeroBasis = false, float MaxDistance = -1.0f);

	/** 현재 커서가 위치한 곳의 타일좌표를 가져옵니다. 양수 값의 사거리를 매개변수로 넘겨주면 사거리 내의 타일의 좌표를 가져옵니다. 실패시 false를 반환합니다. */
	bool GetTileLocationFromCursor(FVector& OutTileLocation, float MaxDistance = -1.0f);

	/** 액터를 숨깁니다. 리플리케이트 됩니다. */
	UFUNCTION(Server, Reliable)
	void ServerSetActorHiddenInGame(bool bNewIsActorHidden);

	/** 액터의 콜리전을 설정합니다. 리플리케이트 됩니다.*/
	UFUNCTION(Server, Reliable)
	void ServerSetCollisionEnabled(bool bNewIsCollisionEnabled);

	/** 움직임을 잠급니다. 리플리케이트 됩니다. */
	UFUNCTION(Server, Reliable)
	void ServerSetMovementEnabled(bool bNewIsMovementEnabled);

	/** 컨트롤 로테이션을 기준으로 캐릭터를 회전시킬지 여부를 나타냅니다. 서버에서 호출되어야합니다. */
	UFUNCTION(Server, Reliable)
	void ServerSetUseControllerRotation(bool bNewUseControllerRotation);

	/** 캐릭터 상태 위젯의 가시성을 조절합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCharacterStateVisibility(bool bNewVisibility);

	/** 다른 클라이언트들에게 인디케이터를 추가합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddScreenIndicatorToSelf(AActor* TargetActor);

	/** 다른 클라이언트들에게서 인디케이터를 제거합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveScreenIndicatorToSelf(AActor* TargetActor);

	/** 해당 클라이언트에서만 인디케이터를 제거합니다. */
	UFUNCTION(Client, Reliable)
	void ClientRemoveScreenIndicatorToSelf(AActor* TargetActor);

	// 버프를 설정하고 제거 시 GC 자동 제거를 처리하는 함수
	void SetLastAppliedBuffAndGCTag(FActiveGameplayEffectHandle NewBuffHandle, FGameplayTag NewGCTag);

	// 버프가 제거될 때 호출되는 델리게이트 핸들러
	UFUNCTION()
	void OnBuffRemoved(const FGameplayEffectRemovalInfo& RemovalInfo);

	UFUNCTION(BlueprintPure, Category = "StereoMix|Character")
	virtual AActor* GetLastAttacker() const override { return LastAttacker.Get(); }

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount, const FGameplayTagContainer& AggregatedSourceTags) override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDamageReceived(AActor* NewAttacker, float InDamageAmount, const FGameplayTagContainer& AggregatedSourceTags);

	virtual bool CanIgnoreAttack() const override;

	virtual bool IsObstacle() const override { return false; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetYawRotation(float InYaw);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetLocation(const FVector_NetQuantize10& NewLocation);

	virtual void Landed(const FHitResult& Hit) override;

	virtual void FocusToCursor();

	UFUNCTION(Client, Reliable)
	void ClientRPCCharacterPushBack(FVector_NetQuantize10 Velocity);

	UFUNCTION(Server, Reliable)
	void ServerRPCCharacterPushBack(FVector_NetQuantize10 Velocity);

	void AddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UFUNCTION(Client, Reliable)
	void ClientRPCAddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UFUNCTION()
	void OnTeamChanged(AActor* TeamActor, ESMTeam PreviousTeam, ESMTeam NewTeam);

	UFUNCTION(BlueprintPure, Category = "StereoMix|Character")
	ESMCharacterType GetCharacterType() const { return CharacterType; }

	ASMNoteBase* GetNote() { return Note; }

	/** 노트 상태인지 여부를 반환합니다. */
	UFUNCTION(BlueprintCallable)
	bool IsNoteState() const { return bIsNoteState; }

	/** True면 음표 상태로, False면 기본 상태로 설정합니다. 서버에서 호출해야합니다.*/
	UFUNCTION(Server, Reliable)
	void ServerSetNoteState(bool bNewIsNote);

	TArray<UMaterialInterface*> GetOriginalMaterials() const { return OriginalMaterials; }

	UMaterialInterface* GetOriginalOverlayMaterial() const { return OriginalOverlayMaterial; }

	virtual void RecalculateDefaultStencil() override;

	UStaticMeshComponent* GetFloorCircleMeshComponent() const { return FloorCircleMeshComponent; }

	FOnCharacterLandedSignature OnCharacterLanded;
	FSimpleMulticastDelegate OnASCInitialized;

protected:
	void Move(const FInputActionValue& InputActionValue);

	void InitASC();

	void GiveDefaultAbilities();

	void InitStat();

	void GAInputPressed(ESMActiveAbilityType InInputID);

	void GAInputReleased(ESMActiveAbilityType InInputID);

	void InitUI();

	/** 현재 마우스커서가 위치한 곳의 좌표를 반환합니다.
	 * bIsZeroBasis가 true면 캐릭터의 바닥을 기준으로, false면 캐릭터의 중심을 기준으로 계산합니다. */
	FVector GetCursorTargetingPoint(bool bUseZeroBasis = false) const;

	void UpdateCameraLocation();

	void UpdateFocusToCursor();
	
	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);

	UFUNCTION()
	void OnRep_bIsActorHidden();

	UFUNCTION()
	void OnRep_bIsCollisionEnabled();

	UFUNCTION()
	void OnRep_bIsMovementEnabled();

	UFUNCTION()
	void OnRep_bUseControllerRotation();

	UFUNCTION(Server, Reliable)
	void ServerRPCAddMoveSpeed(float MoveSpeedMultiplier, float Duration);

	UFUNCTION()
	void OnRep_bIsNoteState();

	virtual void OnHoldStateEntry()
	{
	}

	virtual void OnHoldStateExit()
	{
	}

	UFUNCTION()
	void OnTilesCaptured(const AActor* CapturedInstigator, int CapturedTileCount);

	void UpdateStencilBeforeRoundStart();

	UPROPERTY(EditAnywhere, Category = "Design|Data")
	TObjectPtr<const USMPlayerCharacterDataAsset> DataAsset;

	TWeakObjectPtr<USMAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TObjectPtr<UCapsuleComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Listener")
	TObjectPtr<USceneComponent> ListenerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "Note")
	TObjectPtr<USceneComponent> NoteSlotComponent;

	UPROPERTY(VisibleAnywhere, Category = "Timer")
	TObjectPtr<USMNeutralizeTimerComponent> NeutralizeTimerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Landing")
	TObjectPtr<USMCharacterLandingComponent> LandingComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USMCharacterLevelTransitionVisualComponent> LevelTransitionVisualComponent;

	UPROPERTY(Replicated)
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UStaticMeshComponent> DirectionIndicatorMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UStaticMeshComponent> FloorCircleMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TObjectPtr<UWidgetComponent> NameplateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> DefaultMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> CatchMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> ImmuneMoveTrailFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Emoticon")
	TObjectPtr<USMEmoticonWidgetComponent> EmoticonWidgetComponent; 

	UPROPERTY()
	TObjectPtr<ASMPlayerController> SMPlayerController;

	UPROPERTY()
	TObjectPtr<USMHIC_Character> HIC;

	/** 공격을 무시하는 태그들입니다. */
	UPROPERTY(EditAnywhere, Category = "Desing|GAS|Tag")
	FGameplayTagContainer IgnoreAttackTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockAimTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockCameraTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer LockMovementTags;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|Tag")
	FGameplayTagContainer PushBackImmuneTags;

	/** 캐릭터의 원본 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	/** 캐릭터의 원본 오버레이 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TObjectPtr<UMaterialInterface> OriginalOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design|Socket")
	FName TrailSocket = TEXT("Trail-Point");

	/** 카메라 이동에 사용될 마우스 인식 범위입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMouseThreshold = 1150.0f;

	/** 카메라의 최대 이동 거리입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Camera")
	float CameraMoveMaxDistance = 250.0f;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsActorHidden")
	uint32 bIsActorHidden : 1 = false;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsCollisionEnabled")
	uint32 bIsCollisionEnabled : 1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsMovementEnabled")
	uint32 bIsMovementEnabled : 1 = true;

	UPROPERTY(ReplicatedUsing = "OnRep_bUseControllerRotation")
	uint32 bUseControllerRotation : 1 = true;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttacker;

	UPROPERTY(Replicated)
	FGameplayTagContainer LastAttackTags;

	ESMCharacterType CharacterType = ESMCharacterType::None;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsNoteState")
	uint32 bIsNoteState : 1 = false;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SkillGaugeHealAmountPerCapture = 10.0f;

	UPROPERTY(VisibleAnywhere, Category = "Face")
	TObjectPtr<USMFaceControlComponent> FaceControlComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	float DirectionIndicatorRadius = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float FloorCircleRadius = 100.0f;

	FActiveGameplayEffectHandle LastAppliedBuffHandle;

	FGameplayTag LastAppliedGCTag;

	FTimerHandle StencilUpdateTimerHandle;

	bool bUserInterfaceInitialized = false;
};
