// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Buff.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Audio/SMAudioManager.h"
#include "Audio/SMFMODBlueprintStatics.h"
#include "Camera/CameraComponent.h"
#include "Data/DataAsset/SMBuffDataAsset.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Messages/SMAbilityMessages.h"
#include "UI/GameplayPop/SMGameplayPopSubsystem.h"


ASMGCNA_Buff::ASMGCNA_Buff()
{
	PrimaryActorTick.bCanEverTick = false;

	MusicEventInstance.Instance = nullptr;
}

void ASMGCNA_Buff::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UFMODBlueprintStatics::EventInstanceIsValid(MusicEventInstance))
	{
		UFMODBlueprintStatics::EventInstanceStop(MusicEventInstance, true);
	}
}

bool ASMGCNA_Buff::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const ASMPlayerCharacterBase* OwnerCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!OwnerCharacter)
	{
		return false;
	}

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();

	if (OwnerCharacter->IsLocallyControlled())
	{
		UCameraComponent* CameraComponent = OwnerCharacter->GetComponentByClass<UCameraComponent>();
		UNiagaraSystem* CachedVFX = VFX.Contains(OwnerTeam) ? VFX[OwnerTeam] : nullptr;
		if (CameraComponent && CachedVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(CachedVFX, CameraComponent, NAME_None, FVector(300.0, 0.0, 0.0), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
		}

		if (SFX.Find(OwnerTeam))
		{
			MusicEventInstance = USMFMODBlueprintStatics::PlayEvent2D(MyTarget, SFX[OwnerTeam], true);
		}
	}

	USceneComponent* OwnerRoot = OwnerCharacter->GetRootComponent();
	UNiagaraSystem* CachedLoopVFX = LoopVFX.Contains(OwnerTeam) ? LoopVFX[OwnerTeam] : nullptr;
	if (OwnerRoot && CachedLoopVFX)
	{
		if (LoopVFXComponent)
		{
			LoopVFXComponent->ReleaseToPool();
			LoopVFXComponent->Deactivate();
			LoopVFXComponent = nullptr;
		}

		if (LoopVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(CachedLoopVFX, OwnerRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease); LoopVFXComponent)
		{
			LoopVFXComponent->SetAbsolute(false, true, true);
		}
	}

	USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
	UMaterialInterface* CachedOverlayMaterial = OverlayMaterial.Contains(OwnerTeam) ? OverlayMaterial[OwnerTeam] : nullptr;
	if (OwnerMesh && CachedOverlayMaterial)
	{
		OwnerMesh->SetOverlayMaterial(CachedOverlayMaterial);
	}

	NotifyBuffAdd(MyTarget, Parameters.RawMagnitude);
	return true;
}

bool ASMGCNA_Buff::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (LoopVFXComponent)
	{
		LoopVFXComponent->ReleaseToPool();
		LoopVFXComponent->Deactivate();
		LoopVFXComponent = nullptr;
	}

	const ASMPlayerCharacterBase* OwnerCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!OwnerCharacter)
	{
		return false;
	}

	if (MusicEventInstance.Instance && OwnerCharacter->IsLocallyControlled())
	{
		UFMODBlueprintStatics::EventInstanceStop(MusicEventInstance, true);
	}

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();

	if (OverlayMaterial.Contains(OwnerTeam))
	{
		if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
		{
			OwnerMesh->SetOverlayMaterial(OwnerCharacter->GetOriginalOverlayMaterial());
		}
	}

	NotifyBuffRemove(MyTarget);
	return true;
}

void ASMGCNA_Buff::NotifyBuffAdd(AActor* Target, float Duration)
{
	if (!BuffData)
	{
		UE_LOG(LogStereoMix, Error, TEXT("BuffData is not set in %s"), *GetName());
		return;
	}
	FSMBuffMessage Message;
	Message.Target = Target;
	Message.BuffTag = BuffData->BuffTag;
	Message.Data = BuffData;
	Message.Duration = Duration;
	Message.bAdded = true;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.BuffTag, Message);

	if (USMGameplayPopSubsystem* PopSubsystem = USMGameplayPopSubsystem::Get(this))
	{
		PopSubsystem->QueueActivateBuffPop(Target, Target->GetActorLocation(), BuffData->DisplayName, false);
	}
}

void ASMGCNA_Buff::NotifyBuffRemove(AActor* Target) const
{
	if (!BuffData)
	{
		UE_LOG(LogStereoMix, Error, TEXT("BuffData is not set in %s"), *GetName());
		return;
	}

	FSMBuffMessage Message;
	Message.Target = Target;
	Message.BuffTag = BuffData->BuffTag;
	Message.Data = BuffData;
	Message.bAdded = false;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.BuffTag, Message);
}
