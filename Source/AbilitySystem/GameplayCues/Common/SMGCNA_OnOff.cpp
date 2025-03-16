// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_OnOff.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"


bool ASMGCNA_OnOff::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!SourceCharacter)
	{
		return false;
	}

	FVector TargetLocation;
	FRotator TargetRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, TargetLocation, TargetRotation);
	USceneComponent* TargetComponent = Parameters.TargetAttachComponent.Get();

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (VFX.Find(SourceTeam))
	{
		FFXSystemSpawnParameters NiagaraParams;
		NiagaraParams.WorldContextObject = SourceCharacter;
		NiagaraParams.SystemTemplate = VFX[SourceTeam];
		NiagaraParams.AttachToComponent = TargetComponent;
		NiagaraParams.Location = TargetLocation;
		NiagaraParams.Rotation = TargetRotation;
		NiagaraParams.LocationType = EAttachLocation::KeepRelativeOffset;
		NiagaraParams.bAutoDestroy = false;
		NiagaraParams.bAutoActivate = true;
		NiagaraParams.PoolingMethod = EPSCPoolMethod::ManualRelease;

		if (TargetComponent)
		{
			VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(NiagaraParams);
			if (VFXComponent)
			{
				VFXComponent->SetAbsolute(false, !bUseRotationAttach, false);
			}
		}
		else
		{
			VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocationWithParams(NiagaraParams);
		}
	}

	if (SFX.Find(SourceTeam))
	{
		if (TargetComponent)
		{
			SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], TargetComponent, NAME_None, TargetLocation, EAttachLocation::KeepRelativeOffset, true, true, true);
		}
		else
		{
			FTransform TargetTransform(TargetRotation, TargetLocation);
			SFXInstance = UFMODBlueprintStatics::PlayEventAtLocation(SourceCharacter, SFX[SourceTeam], TargetTransform, true);
		}
	}

	return true;
}

bool ASMGCNA_OnOff::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (VFXComponent)
	{
		VFXComponent->ReleaseToPool();
		VFXComponent->Deactivate();
		VFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		SFXComponent->Deactivate();
		SFXComponent = nullptr;
	}

	if (UFMODBlueprintStatics::EventInstanceIsValid(SFXInstance))
	{
		UFMODBlueprintStatics::EventInstanceRelease(SFXInstance);
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!SourceCharacter)
	{
		return false;
	}

	FVector TargetLocation;
	FRotator TargetRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, TargetLocation, TargetRotation);
	USceneComponent* TargetComponent = Parameters.TargetAttachComponent.Get();

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (OffVFX.Find(SourceTeam))
	{
		FFXSystemSpawnParameters NiagaraParams;
		NiagaraParams.WorldContextObject = SourceCharacter;
		NiagaraParams.SystemTemplate = OffVFX[SourceTeam];
		NiagaraParams.AttachToComponent = TargetComponent;
		NiagaraParams.Location = TargetLocation;
		NiagaraParams.Rotation = TargetRotation;
		NiagaraParams.LocationType = EAttachLocation::KeepRelativeOffset;
		NiagaraParams.bAutoDestroy = false;
		NiagaraParams.bAutoActivate = true;
		NiagaraParams.PoolingMethod = EPSCPoolMethod::AutoRelease;

		if (TargetComponent)
		{
			if (UNiagaraComponent* OffVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(NiagaraParams))
			{
				OffVFXComponent->SetAbsolute(false, !bUseRotationAttach, false);
			}
		}
		else
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocationWithParams(NiagaraParams);
		}
	}

	if (OffSFX.Find(SourceTeam))
	{
		if (TargetComponent)
		{
			UFMODBlueprintStatics::PlayEventAttached(OffSFX[SourceTeam], TargetComponent, NAME_None, TargetLocation, EAttachLocation::KeepRelativeOffset, true, true, true);
		}
		else
		{
			FTransform TargetTransform(TargetRotation, TargetLocation);
			UFMODBlueprintStatics::PlayEventAtLocation(SourceCharacter, OffSFX[SourceTeam], TargetTransform, true);
		}
	}

	return true;
}

void ASMGCNA_OnOff::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SFXComponent)
	{
		SFXComponent->Stop();
		SFXComponent->Release();
		SFXComponent->Deactivate();
		SFXComponent = nullptr;
	}
	
	if (UFMODBlueprintStatics::EventInstanceIsValid(SFXInstance))
	{
		UFMODBlueprintStatics::EventInstanceRelease(SFXInstance);
	}
}
