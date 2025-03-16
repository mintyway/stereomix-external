﻿// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_TutorialAI.h"

#include "SMHIC_Character.h"
#include "Actors/Character/AI/SMAICharacterBase.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Actors/Character/Player/SMElectricGuitarCharacter.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"


bool USMHIC_TutorialAI::CanBeHeld(AActor* Instigator) const
{
	if (!SourceCharacter)
	{
		return false;
	}

	if (HeldMeActorsHistory.Contains(Instigator))
	{
		return false;
	}

	if (!SourceCharacter->IsNoteState())
	{
		return false;
	}

	return true;
}

void USMHIC_TutorialAI::BeginPlay()
{
	Super::BeginPlay();

	SourceCharacter = GetOwner<ASMAICharacterBase>();
}

void USMHIC_TutorialAI::OnHeld(AActor* Instigator)
{
	if (!SourceCharacter)
	{
		return;
	}

	SourceCharacter->SetActorHiddenInGame(true);
	SourceCharacter->SetActorEnableCollision(false);

	if (ASMNoteBase* OwnerNote = SourceCharacter->GetNote())
	{
		OwnerNote->SetActorHiddenInGame(true);
	}

	SourceCharacter->RemoveScreenIndicatorFromSelf(SourceCharacter);

	SourceCharacter->AttachToActor(Instigator, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HeldMeActorsHistory.Add(Instigator);
	SetActorHoldingMe(Instigator);
}

void USMHIC_TutorialAI::OnNoiseBreakStarted(AActor* Instigator)
{
	bIsNoiseBreakStarted = true;
	SourceCharacter->DestroyNoteStateEndTimer();
}

void USMHIC_TutorialAI::OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	const UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !NoiseBreakData || !SourceCapsule)
	{
		return;
	}

	const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
	const FVector NewLocation = NoiseBreakData->NoiseBreakLocation + Offset;
	ReleasedFromBeingHeld(Instigator, NewLocation);
}

void USMHIC_TutorialAI::OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	const UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	if (!SourceCharacter || !NoiseBreakData || !SourceCapsule)
	{
		return;
	}

	const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
	const FVector NewLocation = NoiseBreakData->NoiseBreakLocation + Offset;
	ReleasedFromBeingHeld(Instigator, NewLocation);
}

void USMHIC_TutorialAI::OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	if (!SourceCharacter)
	{
		return;
	}

	ReleasedFromBeingHeld(Instigator);
}

void USMHIC_TutorialAI::OnReleasedFromHold(AActor* Instigator)
{
	AActor* TargetActor = GetActorHoldingMe();
	USMHIC_Character* TargetHIC = Cast<USMHIC_Character>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor));

	// 잡기 상태에서 벗어납니다.
	ReleasedFromBeingHeld(TargetActor);

	if (TargetHIC)
	{
		TargetHIC->SetActorIAmHolding(nullptr);
	}
}

void USMHIC_TutorialAI::ReleasedFromBeingHeld(const AActor* TargetActor, const TOptional<FVector>& TargetOptionalLocation)
{
	if (!SourceCharacter)
	{
		return;
	}

	SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 타겟 캐릭터가 있으면 타겟의 회전값을 사용하고, 없으면 소스 캐릭터의 현재 회전값을 사용하여 소스 캐릭터의 최종 회전을 설정합니다.
	const float FinalSourceYaw = TargetActor ? TargetActor->GetActorRotation().Yaw : SourceCharacter->GetActorRotation().Yaw;
	SourceCharacter->SetActorRotation(FRotator(0.0, FinalSourceYaw, 0.0));

	// 타겟의 앞으로 위치를 조정합니다. 만약 옵션 위치가 제공되었다면 이를 사용합니다.
	const FVector TargetLocation = TargetActor ? TargetActor->GetActorLocation() : SourceCharacter->GetActorLocation();
	const FVector FinalSourceLocation = TargetOptionalLocation.Get(TargetLocation);
	SourceCharacter->SetActorLocation(FinalSourceLocation);

	SourceCharacter->SetActorHiddenInGame(false);
	SourceCharacter->SetActorEnableCollision(true);

	if (ASMNoteBase* OwnerNote = SourceCharacter->GetNote())
	{
		OwnerNote->SetActorHiddenInGame(false);
	}

	SetActorHoldingMe(nullptr);

	// 탈출 로직
	if (bIsNoiseBreakStarted)
	{
		bIsNoiseBreakStarted = false;

		if (const UWorld* World = GetWorld())
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr = MakeWeakObjectPtr(this)] {
				if (ThisWeakPtr.IsValid())
				{
					if (ASMAICharacterBase* CachedCharacter = ThisWeakPtr->SourceCharacter)
					{
						CachedCharacter->SetNoteState(false);
						CachedCharacter->SetGhostMaterial(1.0f);
					}

					ThisWeakPtr->ClearHeldMeActorsHistory();
				}
			}, 2.0f, false);
		}
	}
	else
	{
		SourceCharacter->SetNoteState(false);
		SourceCharacter->SetGhostMaterial(1.0f);
		ClearHeldMeActorsHistory();
	}
}

void USMHIC_TutorialAI::ClearHeldMeActorsHistory()
{
	HeldMeActorsHistory.Empty();
}
