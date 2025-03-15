// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterLevelTransitionVisualComponent.h"

#include "Engine/AssetManager.h"
#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Camera/CameraActor.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMAssetManagerFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SMPlayerController.h"
#include "UI/Transition/SMTransitionManager.h"
#include "Utilities/SMLog.h"


USMCharacterLevelTransitionVisualComponent::USMCharacterLevelTransitionVisualComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void USMCharacterLevelTransitionVisualComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UWorld* World = GetWorld();
	if (!World->IsGameWorld())
	{
		return;
	}

	if (OwnerCharacter = GetOwner<ASMPlayerCharacterBase>(); OwnerCharacter)
	{
		const UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		const FSoftObjectPath OwnerDataAssetPath = AssetManager ? AssetManager->GetPrimaryAssetPath(USMAssetFunctionLibrary::GetCharacterDataAssetID(OwnerCharacter->GetCharacterType())) : nullptr;
		if (OwnerDataAssetPath.IsValid())
		{
			OwnerDataAsset = Cast<USMPlayerCharacterDataAsset>(OwnerDataAssetPath.TryLoad());
		}
	}

	const USMCommonCharacterDataAsset* CommonDataAsset = OwnerDataAsset ? OwnerDataAsset->GetCommonDataAsset() : nullptr;
	UNiagaraSystem* TeleportUpVFX = CommonDataAsset ? CommonDataAsset->TeleportUpVFX : nullptr;
	VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TeleportUpVFX, OwnerCharacter->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, false, ENCPoolMethod::AutoRelease);

	TArray<AActor*> CameraActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), TEXT("AllView"), CameraActors);

	AllViewCamera = CameraActors.IsValidIndex(0) ? Cast<ACameraActor>(CameraActors[0]) : nullptr;
}

void USMCharacterLevelTransitionVisualComponent::ActivateOnServer()
{
	if (!HasAuthority())
	{
		return;
	}

	Activate(true);
	ClientActivate();
}

void USMCharacterLevelTransitionVisualComponent::ClientActivate_Implementation()
{
	Activate(true);
}

void USMCharacterLevelTransitionVisualComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	// 현재는 시퀀스의 타이밍으로 하드코딩되어있습니다.
	constexpr float FramePerSeconds = 30.0f;

	if (HasAuthority())
	{
		FTimerHandle ChangeToNoteTimerHandle;
		constexpr float ChangeToNoteTime = 62.0f / FramePerSeconds;
		GetWorldTimerManager().SetTimer(ChangeToNoteTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this] {
			if (USMAbilitySystemComponent* OwnerASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(OwnerCharacter))
			{
				OwnerASC->AddTag(SMTags::Character::State::Common::Uncontrollable);
			}

			ChangeToNote();
		}), ChangeToNoteTime, false);
	}

	if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
	{
		if (ASMPlayerController* OwnerController = OwnerCharacter->GetController<ASMPlayerController>())
		{
			constexpr float CameraBlendTime = 0.25f;
			OwnerController->SetViewTargetWithBlend(AllViewCamera, CameraBlendTime, VTBlend_Cubic);
		}

		FTimerHandle TeleportStartTimerHandle;
		constexpr float TeleportStartTime = 86.0f / FramePerSeconds;
		GetWorldTimerManager().SetTimer(TeleportStartTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::PlayFX), TeleportStartTime, false);

		FTimerHandle SequenceEndTimerHandle;
		constexpr float SequenceEndTime = 181.0f / FramePerSeconds;
		GetWorldTimerManager().SetTimer(SequenceEndTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnSequenceEnded), SequenceEndTime, false);
	}
}

void USMCharacterLevelTransitionVisualComponent::PlayFX()
{
	for (ASMPlayerCharacterBase* PlayerCharacter : TActorRange<ASMPlayerCharacterBase>(GetWorld()))
	{
		const USMCharacterLevelTransitionVisualComponent* LevelTransitionComponent = PlayerCharacter->GetComponentByClass<ThisClass>();
		if (UNiagaraComponent* CachedVFXComponent = LevelTransitionComponent ? LevelTransitionComponent->VFXComponent : nullptr)
		{
			PlayerCharacter->SetActorHiddenInGame(true);
			CachedVFXComponent->Activate(true);
		}
	}
}

void USMCharacterLevelTransitionVisualComponent::OnSequenceEnded()
{
	if (USMTransitionManager* TransitionManager = USMTransitionManager::Get(this))
	{
		NET_LOG(GetOwner(), Warning, TEXT("트랜지션 시작"));
		TransitionManager->DoTransition(ESMTransitionType::In, this, &ThisClass::OnTransitionEndedCallback);
	}
}

void USMCharacterLevelTransitionVisualComponent::OnTransitionEndedCallback()
{
	bIsTransitionEnded = true;
	TransitionEnd();
	ServerTransitionEnd();
}

void USMCharacterLevelTransitionVisualComponent::TransitionEnd()
{
	bIsTransitionEnded = true;
	OnTransitionEnded.Broadcast();
}

void USMCharacterLevelTransitionVisualComponent::ServerTransitionEnd_Implementation()
{
	NET_LOG(GetOwner(), Warning, TEXT("캐릭터 트랜지션 종료 콜백"));
	TransitionEnd();
}

void USMCharacterLevelTransitionVisualComponent::ChangeToNote()
{
	if (OwnerCharacter && OwnerCharacter->HasAuthority())
	{
		NET_LOG(GetOwner(), Warning, TEXT("노트로 변환"));
		OwnerCharacter->ServerSetNoteState(true);
	}
}
