﻿// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "SMHIC_TutorialAI.generated.h"


class ASMAICharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_TutorialAI : public USMHoldInteractionComponent
{
	GENERATED_BODY()

public:
	virtual bool ShouldCaptureTilesFromNoiseBreak() const override { return true; }
	
	virtual bool CanBeHeld(AActor* Instigator) const override;

	virtual void OnHeld(AActor* Instigator) override;

	virtual void OnNoiseBreakStarted(AActor* Instigator) override;
	
	virtual void OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnReleasedFromHold(AActor* Instigator) override;

	void ReleasedFromBeingHeld(const AActor* TargetActor, const TOptional<FVector>& TargetOptionalLocation = TOptional<FVector>());
	
	void ClearHeldMeActorsHistory();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<ASMAICharacterBase> SourceCharacter;
	
	TArray<TWeakObjectPtr<AActor>> HeldMeActorsHistory;

	uint32 bIsNoiseBreakStarted:1;
};
