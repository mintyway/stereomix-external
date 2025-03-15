// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterLandingComponent.h"

#include "EngineUtils.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "LevelSequencePlayer.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "SMGameplayTags.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/Enum/SMRoundState.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Games/SMGameState.h"
#include "Games/Round/SMRoundComponent.h"
#include "System/SMWorldSettings.h"


USMCharacterLandingComponent::USMCharacterLandingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void USMCharacterLandingComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// #if WITH_EDITOR
	// 	if (!bUseLandingInEditor)
	// 	{
	// 		return;
	// 	}
	// #endif

	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}

	if (const ASMWorldSettings* WorldSettings = Cast<ASMWorldSettings>(World->GetWorldSettings()))
	{
		if (WorldSettings->GameplayTags.HasTag(StereoMix::Tags::Character_DoNotLanding))
		{
			return;
		}
	}

	if (OwnerCharacter = GetOwner<ASMPlayerCharacterBase>(); OwnerCharacter)
	{
		OwnerCharacter->OnASCInitialized.AddUObject(this, &ThisClass::OnASCInitialized);
	}

	if (AGameStateBase* GameState = World->GetGameState()) // 게임 스테이트에서 필요한 이벤트를 바인드합니다.
	{
		BindGameState(GameState);
	}
	else // 클라이언트는 게임 스테이트가 좀 늦게 생성되는데 이를 위한 이벤트 입니다.
	{
		World->GameStateSetEvent.AddUObject(this, &ThisClass::BindGameState);
	}
}

void USMCharacterLandingComponent::SetCharacterActivation(bool bIsActivated)
{
	if (!OwnerCharacter || GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	// 한번이라도 활성화 된 경우 비활성화가 불가능하도록 플래그를 사용합니다.
	// 이유는 이미 인 라운드인 경우 SetCharacterActivation가 즉시 true로 호출되는데 이때 ASC가 GameState보다 늦게 초기화 되어 false로 덮어씌워지기 때문입니다.
	if (bIsStarted)
	{
		return;
	}
	if (bIsActivated)
	{
		bIsStarted = true;
	}

	OwnerCharacter->ServerSetActorHiddenInGame(!bIsActivated);
	OwnerCharacter->ServerSetUseControllerRotation(bIsActivated);

	if (USMAbilitySystemComponent* OwnerASC = OwnerCharacter->GetAbilitySystemComponent<USMAbilitySystemComponent>())
	{
		if (bIsActivated)
		{
			OwnerASC->RemoveTag(SMTags::Character::State::Common::Uncontrollable);
		}
		else
		{
			OwnerASC->AddTag(SMTags::Character::State::Common::Uncontrollable);
		}
	}
}

void USMCharacterLandingComponent::OnASCInitialized()
{
	SetCharacterActivation(false);
}

void USMCharacterLandingComponent::OnCountdownStarted()
{
	PlayCountdownLevelSequence();
	StartLanding();
}

void USMCharacterLandingComponent::OnRoundStateUpdated(ESMRoundState PreviousRoundState, ESMRoundState NewRoundState)
{
	if (NewRoundState == ESMRoundState::PreRound)
	{
		OnCountdownStarted();
	}
	else if (NewRoundState == ESMRoundState::InRound)
	{
		OnInRoundStarted();
	}
}

void USMCharacterLandingComponent::OnInRoundStarted()
{
	SetCharacterActivation(true);
}

void USMCharacterLandingComponent::BindGameState(AGameStateBase* InGameState)
{
	if (UWorld* World = GetWorld())
	{
		World->GameStateSetEvent.AddUObject(this, &ThisClass::BindGameState);
	}

	ASMGameState* GameState = Cast<ASMGameState>(InGameState);
	if (!GameState)
	{
		return;
	}

	if (USMRoundComponent* RoundComponent = USMRoundComponent::Get(this))
	{
		RoundComponent->OnRoundStateUpdatedEvent.AddUObject(this, &ThisClass::OnRoundStateUpdated);
		const ESMRoundState CurrentRoundState = RoundComponent->GetRoundState();
		if (CurrentRoundState > ESMRoundState::PreRound) // 이미 라운드가 시작된 경우
		{
			OnInRoundStarted();
		}
	}
	else
	{
		if (GetWorld()->bMatchStarted)
		{
			OnInRoundStarted();
		}
		else
		{
			GetWorld()->OnWorldMatchStarting.AddUObject(this, &ThisClass::OnInRoundStarted);
		}
	}
}

void USMCharacterLandingComponent::PlayCountdownLevelSequence()
{
	if (!OwnerCharacter || !OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();

	ASMWorldSettings* WorldSettings = CastChecked<ASMWorldSettings>(GetWorld()->GetWorldSettings());
	if (TSoftObjectPtr<ULevelSequence>* CountdownSequencePtr = WorldSettings->RoundCountdownLevelSequence.Find(OwnerTeam))
	{
		if (ULevelSequence* CountdownSequence = CountdownSequencePtr->LoadSynchronous())
		{
			FMovieSceneSequencePlaybackSettings PlaybackSettings;
			PlaybackSettings.bAutoPlay = true;
			ALevelSequenceActor* LevelSequenceActor;
			ULevelSequencePlayer::CreateLevelSequencePlayer(this, CountdownSequence, PlaybackSettings, LevelSequenceActor);
		}
	}
}

void USMCharacterLandingComponent::StartLanding()
{
	const UWorld* World = GetWorld();
	if (!World || !OwnerCharacter || !OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	TArray<ASMPlayerCharacterBase*> AllyPlayerCharacters;
	TArray<ASMPlayerCharacterBase*> EnemyPlayerCharacters;
	for (ASMPlayerCharacterBase* Character : TActorRange<ASMPlayerCharacterBase>(GetWorld()))
	{
		if (USMTeamBlueprintLibrary::IsSameLocalTeam(Character))
		{
			AllyPlayerCharacters.Add(Character);
		}
		else
		{
			EnemyPlayerCharacters.Add(Character);
		}
	}

	auto CharacterTypeSort = [](const ASMPlayerCharacterBase& LHS, const ASMPlayerCharacterBase& RHS) { return LHS.GetCharacterType() < RHS.GetCharacterType(); };
	AllyPlayerCharacters.Sort(CharacterTypeSort);
	EnemyPlayerCharacters.Sort(CharacterTypeSort);

	auto LandCharacter = [this, World](const TArray<ASMPlayerCharacterBase*>& Characters) {
		for (int32 i = 0; i < Characters.Num(); ++i)
		{
			const ASMPlayerCharacterBase* Character = Characters[i];
			USMCharacterLandingComponent* LandingComponent = Character ? Character->GetComponentByClass<USMCharacterLandingComponent>() : nullptr;
			auto LandLambda = [LandingComponent] { LandingComponent->PlayLanding(); };

			if (i == 0)
			{
				LandLambda();
			}
			else
			{
				FTimerHandle TimerHandle;
				World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(LandingComponent, LandLambda), SpawnInterval * i, false);
			}
		}
	};

	auto AllyLand = [LandCharacter, AllyPlayerCharacters] {
		LandCharacter(AllyPlayerCharacters);
	};

	auto EnemyLand = [LandCharacter, EnemyPlayerCharacters] {
		LandCharacter(EnemyPlayerCharacters);
	};

	const float Interval = CountdownTotalTime / 3.0f;

	FTimerHandle AllyLandTimerHandle;
	World->GetTimerManager().SetTimer(AllyLandTimerHandle, FTimerDelegate::CreateWeakLambda(this, AllyLand), Interval * 2, false);

	FTimerHandle EnemyLandTimerHandle;
	World->GetTimerManager().SetTimer(EnemyLandTimerHandle, FTimerDelegate::CreateWeakLambda(this, EnemyLand), Interval, false);
}

void USMCharacterLandingComponent::PlayLanding()
{
	if (!OwnerCharacter)
	{
		return;
	}

	OwnerCharacter->SetActorHiddenInGame(false);

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();

	// 랜딩 애니메이션을 재생합니다.
	const USMPlayerCharacterDataAsset* OwnerDataAsset = OwnerCharacter->GetDataAsset();
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> LandingMontageMap = OwnerDataAsset ? OwnerDataAsset->LandingMontage : TMap<ESMTeam, TObjectPtr<UAnimMontage>>();
	if (UAnimMontage* LandingMontage = LandingMontageMap.Contains(OwnerTeam) ? LandingMontageMap[OwnerTeam] : nullptr)
	{
		OwnerCharacter->PlayAnimMontage(LandingMontage);
	}

	// 랜딩 이펙트를 재생합니다.
	const USMCommonCharacterDataAsset* OwnerCommonDataAsset = OwnerDataAsset ? OwnerDataAsset->GetCommonDataAsset() : nullptr;
	if (UNiagaraSystem* LandingVFX = OwnerCommonDataAsset ? OwnerCommonDataAsset->LandingDownVFX : nullptr)
	{
		const UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
		const float CapsuleHalfHeight = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
		const FVector Offset(0.0, 0.0, -CapsuleHalfHeight);
		const FVector LandingLocation = OwnerCharacter->GetActorLocation() + Offset;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, LandingVFX, LandingLocation, FRotator::ZeroRotator, FVector(1), false, true, ENCPoolMethod::AutoRelease);
	}

	if (UFMODEvent* LandingSFX = OwnerCommonDataAsset ? OwnerCommonDataAsset->LandingDownSFX : nullptr)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, LandingSFX, true);
	}
}
