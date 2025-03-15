// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "ModularPlayerState.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "Interfaces/SMCharacterInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "System/GameplayTagStack.h"

#include "SMPlayerState.generated.h"

enum class ESMCharacterType : uint8;
enum class ESMTeam : uint8;
struct FGameplayEventData;
struct FSMVerbMessage;
struct FGameplayTag;
class ASMPlayerController;
class USMAbilitySystemComponent;
class USMCharacterAttributeSet;

/**
 * StereoMix Player State
 */
UCLASS(DisplayName = "StereoMix Player State", Category = "StereoMix|PlayerState")
class STEREOMIX_API ASMPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public ISMTeamInterface, public ISMCharacterInterface
{
	GENERATED_BODY()

public:
	ASMPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual FString InitOptions(const FString& Options);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|PlayerState")
	USMAbilitySystemComponent* GetStereoMixAbilitySystemComponent() const { return AbilitySystemComponent; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|PlayerState")
	ASMPlayerController* GetStereoMixPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "StereoMix|PlayerState")
	virtual ESMTeam GetTeam() const override;

	void SetTeam(ESMTeam NewTeam);

	virtual USMTeamComponent* GetTeamComponent() const override;

	FPlayerScoreData GetPlayerScore() const;

	void SetPlayerScore(const FPlayerScoreData& ScoreData);

	void SetTempPlayerScore(const FPlayerScoreData& ScoreData);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestChangeTeam(ESMTeam NewTeam);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestChangeCharacter(ESMCharacterType NewCharacter);

	UFUNCTION(BlueprintPure, Category = "StereoMix|PlayerState")
	virtual ESMCharacterType GetCharacterType() const override;

	virtual void SetCharacterType(ESMCharacterType CharacterType) override;

	virtual FSMOnCharacterChanged* GetOnCharacterChangedDelegate() override;

	// 태그 스택은 대상 플레이어의 특정 스탯 태그에 대한 스택 수를 활용하여 게임의 총 대미지, 노이즈 브레이크 횟수 등 다양한 통계를 추적하는 데 사용됩니다.

	// 태그 스택에 지정된 수의 스택을 추가합니다. (StackCount가 1 미만인 경우 아무것도 수행하지 않음)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 태그 스택에서 지정된 수의 스택을 제거합니다. (StackCount가 1 미만인 경우 아무것도 수행하지 않음)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 지정된 태그의 스택 수를 반환합니다.
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// 태그 스택에 지정된 태그가 있는지 확인합니다.
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// Send a message to just this player
	// (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "StereoMix|PlayerState")
	void ClientBroadcastMessage(const FSMVerbMessage Message);

	void InitTeamAndCharacterType(const FString& Options);

	FSMOnCharacterChanged OnCharacterChanged;

protected:
	virtual void OnClientAbilitySystemGameplayEventReceived(const FGameplayTag& EventTag, const FGameplayEventData& Payload);
	virtual void OnClientAbilitySystemTagUpdated(const FGameplayTag& UpdatedTag, bool bTagExists);
	virtual void OnClientAbilitySystemAttributeValueChanged(const FGameplayAttribute& Attribute, const FGameplayAttributeData& OldValue, const FGameplayAttributeData& NewValue);
	virtual void OnClientAbilitySystemGameplayCueAdded(const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& CueParameters);
	virtual void OnClientAbilitySystemGameplayCueRemoved(const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& CueParameters);
	virtual void OnClientAbilitySystemGameplayCueExecuted(const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& CueParameters);

	UPROPERTY(VisibleAnywhere, Category = "GAS|ASC")
	TObjectPtr<USMAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "GAS|AttributeSet")
	TObjectPtr<USMCharacterAttributeSet> CharacterAttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(Replicated)
	FPlayerScoreData PlayerScoreData;

	UPROPERTY(Replicated)
	FPlayerScoreData TempPlayerScoreData;

private:
	UFUNCTION()
	void OnRep_MyCharacterType(ESMCharacterType OldCharacterType);

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|PlayerState")
	ESMTeam DefaultTeam;

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|PlayerState")
	ESMCharacterType DefaultCharacterType;

	UPROPERTY(ReplicatedUsing=OnRep_MyCharacterType)
	ESMCharacterType MyCharacterType;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
};
