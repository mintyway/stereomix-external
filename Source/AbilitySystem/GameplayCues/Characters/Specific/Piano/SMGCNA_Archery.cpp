// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Archery.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Attack/SMGA_Archery.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"

const FName ASMGCNA_Archery::ChargeRatioParameterName(TEXT("ChargeRatio"));
const FName ASMGCNA_Archery::MaxDistanceParameterName(TEXT("MaxRange"));
const FName ASMGCNA_Archery::TeamParameterName(TEXT("Team"));

ASMGCNA_Archery::ASMGCNA_Archery()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASMGCNA_Archery::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SFXComponent)
	{
		SFXComponent->Deactivate();
		SFXComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ASMGCNA_Archery::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!OwnerCharacter.IsValid())
	{
		return;
	}

	if (IndicatorVFXComponent)
	{
		ChargeElapsedTime += DeltaSeconds;
		const float Ratio = ChargeElapsedTime / TimeToChargeLevel2;
		IndicatorVFXComponent->SetFloatParameter(ChargeRatioParameterName, Ratio);
	}
}

bool ASMGCNA_Archery::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	OwnerCharacter = Cast<ASMPianoCharacter>(MyTarget);
	USceneComponent* WeaponMesh = Parameters.TargetAttachComponent.Get();
	if (!OwnerCharacter.IsValid() || !WeaponMesh)
	{
		return false;
	}

	SetActorTickEnabled(true);

	const ESMTeam SourceTeam = OwnerCharacter->GetTeam();

	if (VFX.Find(SourceTeam))
	{
		VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], WeaponMesh, NAME_None, Offset, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (SFX.Find(SourceTeam))
	{
		SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], WeaponMesh, NAME_None, Offset, EAttachLocation::KeepRelativeOffset, true, true, true);
	}

	if (OwnerCharacter->IsLocallyControlled() && IndicatorVFX)
	{
		USceneComponent* AttachComponent = OwnerCharacter->GetRootComponent();
		if (IndicatorVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(IndicatorVFX, AttachComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease); IndicatorVFXComponent)
		{
			IndicatorVFXComponent->SetFloatParameter(ChargeRatioParameterName, 0.0f);

			const ESMCharacterType CharacterType = OwnerCharacter->GetCharacterType();
			const FSMCharacterAttackData* AttackData = USMDataTableFunctionLibrary::GetCharacterAttackData(CharacterType);
			MaxDistance = AttackData ? AttackData->DistanceByTile * USMTileFunctionLibrary::DefaultTileSize : 0.0f;
			IndicatorVFXComponent->SetFloatParameter(MaxDistanceParameterName, MaxDistance);

			const int32 TeamParameterValue = static_cast<int32>(SourceTeam) - 1;
			IndicatorVFXComponent->SetIntParameter(TeamParameterName, TeamParameterValue);

			const USMAbilitySystemComponent* OwnerASC = OwnerCharacter->GetAbilitySystemComponent<USMAbilitySystemComponent>();
			if (const USMGA_Archery* ArcheryInstance = OwnerASC ? OwnerASC->GetGAInstanceFromClass<USMGA_Archery>() : nullptr)
			{
				TimeToChargeLevel1 = ArcheryInstance->GetTimeToChargeLevel1();
				TimeToChargeLevel2 = ArcheryInstance->GetTimeToChargeLevel2();
			}

			ChargeElapsedTime = 0.0f;
		}
	}

	OwnerCharacter->SetWeaponVFXEnabled(false);

	return true;
}

bool ASMGCNA_Archery::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	SetActorTickEnabled(false);

	const int32 ChargedLevel = static_cast<int32>(Parameters.RawMagnitude);

	if (VFXComponent)
	{
		VFXComponent->Deactivate();
		VFXComponent->ReleaseToPool();
		VFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		const FName ParameterName = TEXT("ArrowCharge");
		const float ParameterValue = FMath::Clamp(ChargedLevel + 1, 0.0f, 2.0f);
		SFXComponent->SetParameter(ParameterName, ParameterValue);

		SFXComponent->Stop();
		SFXComponent = nullptr;
	}

	if (IndicatorVFXComponent)
	{
		IndicatorVFXComponent->Deactivate();
		IndicatorVFXComponent->ReleaseToPool();
		IndicatorVFXComponent = nullptr;
	}

	OwnerCharacter = nullptr;
	ChargeElapsedTime = 0.0f;
	MaxDistance = 0.0f;

	// 노트 상태 즉 무력화 상태가 아닌 경우만 이펙트를 다시 활성화합니다.
	ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget);
	if (SourceCharacter ? !SourceCharacter->IsNoteState() : false)
	{
		SourceCharacter->SetWeaponVFXEnabled(true);
	}

	return true;
}
