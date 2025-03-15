// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Neutralize.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Common/SMGA_Hold.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/SMNeutralizeTimerComponent.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Games/Round/SMRoundComponent.h"
#include "Utilities/SMCollision.h"

ASMGCNA_Neutralize::ASMGCNA_Neutralize()
{
	PrimaryActorTick.bCanEverTick = true;

	bAutoAttachToOwner = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	NeutralizeTimerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NeutralizeTimerMesh"));
	NeutralizeTimerMesh->SetupAttachment(RootComponent);
	NeutralizeTimerMesh->SetAbsolute(false, true);
	NeutralizeTimerMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		EndVFX.Add(Key, nullptr);
		ScreenVFX.Add(Key, nullptr);
		StartSFX.Add(Key, nullptr);
		EndSFX.Add(Key, nullptr);
	}
}

void ASMGCNA_Neutralize::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	NeutralizeTimerMesh->CreateAndSetMaterialInstanceDynamic(0);
}

void ASMGCNA_Neutralize::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SFXComponent)
	{
		SFXComponent->Deactivate();
		SFXComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ASMGCNA_Neutralize::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateFloorCircle();
}

bool ASMGCNA_Neutralize::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	OwnerCharacter = Cast<ASMCharacterBase>(MyTarget);
	USceneComponent* OwnerRoot = OwnerCharacter.IsValid() ? OwnerCharacter->GetRootComponent() : nullptr;
	ASMNoteBase* OwnerNote = GetNote(OwnerCharacter.Get());
	const USkeletalMeshComponent* OwnerNoteMesh = OwnerNote ? OwnerNote->GetMesh() : nullptr;
	if (!OwnerCharacter.IsValid() || !OwnerRoot || !OwnerNoteMesh)
	{
		return false;
	}

	ActivateTimer();

	const ESMTeam OwnerTeam = USMTeamBlueprintLibrary::GetTeam(OwnerCharacter.Get());

	if (VFX.Contains(OwnerTeam))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[OwnerTeam], OwnerRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	if (StartSFX.Contains(OwnerTeam))
	{
		UFMODBlueprintStatics::PlayEventAttached(StartSFX[OwnerTeam], OwnerRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, true);
	}

	if (OwnerCharacter->IsLocallyControlled())
	{
		if (ScreenVFX.Contains(OwnerTeam))
		{
			if (UCameraComponent* SourceCamera = OwnerCharacter->GetComponentByClass<UCameraComponent>())
			{
				ScreenVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ScreenVFX[OwnerTeam], SourceCamera, NAME_None, FVector(300.0, 0.0, 0.0), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
			}
		}

		if (SFX.Contains(OwnerTeam))
		{
			SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[OwnerTeam], OwnerRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, true);
		}
	}

	return true;
}

bool ASMGCNA_Neutralize::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (ScreenVFXComponent)
	{
		ScreenVFXComponent->Deactivate();
		ScreenVFXComponent->ReleaseToPool();
		ScreenVFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		SFXComponent->Deactivate();
		SFXComponent = nullptr;
	}

	DeactivateTimer();

	USceneComponent* SourceRoot = OwnerCharacter.IsValid() ? OwnerCharacter->GetRootComponent() : nullptr;
	ASMNoteBase* OwnerNote = GetNote(OwnerCharacter.Get());
	const USkeletalMeshComponent* SourceNoteMesh = OwnerNote ? OwnerNote->GetMesh() : nullptr;
	if (!OwnerCharacter.IsValid() || !SourceRoot || !SourceNoteMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	auto PlayEndFX = [this, SourceRoot, SourceTeam] {
		if (EndVFX.Find(SourceTeam))
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(EndVFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
		}

		if (EndSFX.Find(SourceTeam))
		{
			UFMODBlueprintStatics::PlayEventAttached(EndSFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, true, true);
		}
	};

	if (const USMRoundComponent* RoundComponent = USMRoundComponent::Get(MyTarget))
	{
		if (RoundComponent->GetRoundState() != ESMRoundState::PostRound)
		{
			PlayEndFX();
		}
	}
	else // 테스트 레벨을 위한 예외처리입니다.
	{
		PlayEndFX();
	}

	return true;
}

void ASMGCNA_Neutralize::UpdateTimerRatio(float NewRatio)
{
	if (NeutralizeTimerMesh)
	{
		constexpr float AdjustValue = 0.05f;
		const float AdjustedRatio = ((1.0f + AdjustValue) * NewRatio) - AdjustValue;
		const FName ParameterName = TEXT("Bar");
		NeutralizeTimerMesh->SetScalarParameterValueOnMaterials(ParameterName, AdjustedRatio);
	}
}

void ASMGCNA_Neutralize::UpdateFloorCircle()
{
	if (!NeutralizeTimerMesh || !FloorCircleComponent.IsValid() || !OwnerCharacter.IsValid() || !LocalCharacter.IsValid())
	{
		return;
	}

	// 잡기 가능한 경우 색을 FloorCircle의 색을 바꿉니다.
	const float DistanceToOwner = FVector::Dist2D(LocalCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation());
	const float HoldableRangeValue = DistanceToOwner <= (HoldDistance - HoldDistanceOffset) ? 1.0f : 0.0f; // 1은 붉은색입니다.
	FloorCircleComponent->SetScalarParameterValueOnMaterials(FloorCircleHoldableRangeParameterName, HoldableRangeValue);

	// 스케일의 보간값을 구합니다.
	const float InterpolationDifference = ScaleInterpolationEndDistance - ScaleInterpolationStartDistance;
	const float InterpolationAlpha = FMath::Clamp(1.0f - ((DistanceToOwner - ScaleInterpolationStartDistance) / InterpolationDifference), 0.0f, 1.0f);
	FloorCircleComponent->SetScalarParameterValueOnMaterials(FloorCircleExpandStateColorParameterName, InterpolationAlpha); // 1로 갈수록 흰색 0으로 갈 수록 검정색입니다.

	// 보간값을 통해 실제로 적용되야할 최종 스케일을 구하고 적용합니다.
	const float MaxScale = GetMaxDistanceScale() * InterpolationAlpha;
	const float FinalScale = MaxScale > DefaultTimerScale ? MaxScale : DefaultTimerScale;
	NeutralizeTimerMesh->SetRelativeScale3D(FVector(FinalScale));
	FloorCircleComponent->SetRelativeScale3D(FVector(FinalScale));
}

void ASMGCNA_Neutralize::ActivateTimer()
{
	USMNeutralizeTimerComponent* NeutralizeTimerComponent = OwnerCharacter.IsValid() ? OwnerCharacter->GetComponentByClass<USMNeutralizeTimerComponent>() : nullptr;
	if (!NeutralizeTimerMesh || !NeutralizeTimerComponent)
	{
		return;
	}

	// 로컬 캐릭터를 캐싱합니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	LocalCharacter = LocalPlayerController ? LocalPlayerController->GetPawn<ASMCharacterBase>() : nullptr;

	// 로컬 캐릭터의 잡기 사정거리와 스케일 보간에 필요한 변수들을 캐싱합니다.
	const USMAbilitySystemComponent* LocalASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(LocalCharacter.Get());
	const USMGA_Hold* LocalHoldGAInstance = LocalASC ? LocalASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr;
	HoldDistance = LocalHoldGAInstance ? LocalHoldGAInstance->GetMaxDistance() : 0.0f;
	ScaleInterpolationStartDistance = HoldDistance * ScaleInterpolationStartMultiplier;
	ScaleInterpolationEndDistance = HoldDistance * ScaleInterpolationEndMultiplier;

	// FloorCircle 메시 컴포넌트를 찾고 캐싱합니다.
	FloorCircleComponent = nullptr;
	TArray<UActorComponent*> FloorCircleActorComponents = OwnerCharacter->GetComponentsByTag(UStaticMeshComponent::StaticClass(), TEXT("FloorCircle"));
	for (UActorComponent* FloorCircleActorComponent : FloorCircleActorComponents)
	{
		if (UStaticMeshComponent* FloorCircleComponentAsMeshComponent = Cast<UStaticMeshComponent>(FloorCircleActorComponent))
		{
			FloorCircleComponent = FloorCircleComponentAsMeshComponent;
			break;
		}
	}

	// 캡슐의 크기를 사용해 바닥의 위치를 구하고 타이머를 위치시킵니다.
	const UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
	const float HalfHeight = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
	NeutralizeTimerMesh->SetRelativeLocation(FVector(0.0, 0.0, -HalfHeight));

	// FloorCircle을 기본상태로 리셋합니다.
	ResetFloorCircleState();

	if (!USMTeamBlueprintLibrary::IsSameTeam(OwnerCharacter.Get(), LocalCharacter.Get())) // 로컬 캐릭터의 적인 경우만 FloorCircle을 업데이트합니다.
	{
		SetActorTickEnabled(true);

		// 잡힌 경우 FloorCircle이 리셋되고 업데이트가 중단되어야하기때문에 해당 이벤트를 바인드합니다.
		if (USMHoldInteractionComponent* OwnerHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(OwnerCharacter.Get()))
		{
			OwnerHIC->OnHeldStateEntry.AddUObject(this, &ThisClass::OnHeldStateEntryCallback);
			OwnerHIC->OnHeldStateExit.AddUObject(this, &ThisClass::OnHeldStateExitCallback);
		}
	}

	if (!OwnerCharacter->IsLocallyControlled()) // 로컬은 상시 켜져있으므로 나머지만 켜줍니다.
	{
		if (FloorCircleComponent.IsValid())
		{
			FloorCircleComponent->SetVisibility(true);
		}
	}

	NeutralizeTimerComponent->OnNeutralizeRemainTimeRatio.AddUObject(this, &ASMGCNA_Neutralize::UpdateTimerRatio);
}

void ASMGCNA_Neutralize::DeactivateTimer()
{
	SetActorTickEnabled(false);

	if (USMNeutralizeTimerComponent* NeutralizeTimerComponent = OwnerCharacter.IsValid() ? OwnerCharacter->GetComponentByClass<USMNeutralizeTimerComponent>() : nullptr)
	{
		NeutralizeTimerComponent->OnNeutralizeRemainTimeRatio.RemoveAll(this);
	}

	if (USMHoldInteractionComponent* OwnerHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(OwnerCharacter.Get()))
	{
		OwnerHIC->OnHeldStateEntry.RemoveAll(this);
		OwnerHIC->OnHeldStateExit.RemoveAll(this);
	}

	if (!OwnerCharacter->IsLocallyControlled())
	{
		if (FloorCircleComponent.IsValid())
		{
			FloorCircleComponent->SetVisibility(false);
		}
	}
}

void ASMGCNA_Neutralize::OnHeldStateEntryCallback()
{
	if (!OwnerCharacter.IsValid())
	{
		return;
	}

	// 잡혀있는 동안은 기본 상태로 돌리고 업데이트를 멈춥니다.
	SetActorTickEnabled(false);
	ResetFloorCircleState();

	// 만약 로컬 캐릭터에게 잡힌 경우 더 이상 업데이트 되지 않도록 이벤트를 모두 해제합니다.
	// 즉, 로컬 캐릭터에게 잡힌다면 무력화가 종료될때까지 대상은 기본 CircleFloor를 유지합니다.
	USMHoldInteractionComponent* OwnerHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(OwnerCharacter.Get());
	if (const AActor* HoldingMeActor = OwnerHIC ? OwnerHIC->GetActorHoldingMe() : nullptr)
	{
		if (HoldingMeActor == LocalCharacter.Get())
		{
			OwnerHIC->OnHeldStateEntry.RemoveAll(this);
			OwnerHIC->OnHeldStateExit.RemoveAll(this);
		}
	}
}

void ASMGCNA_Neutralize::OnHeldStateExitCallback()
{
	// 잡힌 상태에서 벗어나면 업데이트를 재개합니다.
	SetActorTickEnabled(true);
}

void ASMGCNA_Neutralize::ResetFloorCircleState()
{
	// NeutralizeTimer를 기본값으로 초기화합니다
	NeutralizeTimerMesh->SetRelativeScale3D(FVector(DefaultTimerScale));

	// FloorCircle을 기본값으로 초기화합니다
	if (FloorCircleComponent.IsValid())
	{
		FloorCircleComponent->SetRelativeScale3D(FVector(DefaultTimerScale));
		FloorCircleComponent->SetScalarParameterValueOnMaterials(FloorCircleHoldableRangeParameterName, 0.0f);
	}
}

ASMNoteBase* ASMGCNA_Neutralize::GetNote(const ASMCharacterBase* InCharacter)
{
	if (InCharacter)
	{
		TArray<AActor*> AttachedActors;
		InCharacter->GetAttachedActors(AttachedActors);

		for (AActor* AttachedActor : AttachedActors)
		{
			if (ASMNoteBase* Note = Cast<ASMNoteBase>(AttachedActor))
			{
				return Note;
			}
		}
	}

	return nullptr;
}

float ASMGCNA_Neutralize::GetMaxDistanceScale()
{
	const float HoldDistanceMaxScale = ((HoldDistance - HoldDistanceOffset) / 100.0f) * 2.0f;
	return HoldDistanceMaxScale > 1.0f ? HoldDistanceMaxScale : 1.0f;
}
