// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/PawnComponent.h"
#include "SMCharacterLevelTransitionVisualComponent.generated.h"


class USMPlayerCharacterDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCharacterLevelTransitionVisualComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	USMCharacterLevelTransitionVisualComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	void ActivateOnServer();

	bool IsTransitionEnded() const { return bIsTransitionEnded; }

	TMulticastDelegate<void()> OnTransitionEnded;

protected:
	UFUNCTION(Client, Reliable)
	void ClientActivate();

	virtual void Activate(bool bReset) override;

	void PlayFX();

	UFUNCTION()
	void OnSequenceEnded();

	void OnTransitionEndedCallback();

	void TransitionEnd();

	UFUNCTION(Server, Reliable)
	void ServerTransitionEnd();

	UPROPERTY()
	TObjectPtr<ASMPlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<USMPlayerCharacterDataAsset> OwnerDataAsset;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	uint32 bIsTransitionEnded:1 = false;

	UPROPERTY()
	TObjectPtr<ACameraActor> AllViewCamera;

private:
	void ChangeToNote();
};
