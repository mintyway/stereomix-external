// Copyright Studio Surround. All Rights Reserved.


#include "SMGameState.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Components/Core/SMCharacterSpawnManagerComponent.h"
#include "Messages/SMVerbMessage.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMPlayerController.h"
#include "Player/SMPlayerState.h"
#include "UI/Transition/SMTransitionManager.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameState)

extern ENGINE_API float GAverageFPS;

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixGameState, Log, All);

ASMGameState::ASMGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<USMAbilitySystemComponent>(this, TEXT("ASC"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void ASMGameState::BeginPlay()
{
	NET_LOG_CATEGORY(this, LogStereoMixGameState, Verbose, TEXT("GameState BeginPlay"));
	Super::BeginPlay();

	if (HasAuthority())
	{
		bIsReadyToStart = false;
	}
}

void ASMGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	NET_LOG_CATEGORY(this, LogStereoMixGameState, Verbose, TEXT("GameState EndPlay"));
	Super::EndPlay(EndPlayReason);
}

void ASMGameState::AddPlayerState(APlayerState* PlayerState)
{
	NET_LOG_CATEGORY(this, LogStereoMixGameState, Verbose, TEXT("AddPlayerState: %s"), *PlayerState->GetName());
	Super::AddPlayerState(PlayerState);
	if (!PlayerState->IsInactive())
	{
		if (ASMPlayerState* StereoMixPS = Cast<ASMPlayerState>(PlayerState))
		{
			OnPlayerAddedEvent.Broadcast(StereoMixPS);
			K2_OnPlayerAddedEvent.Broadcast(StereoMixPS);
		}
	}
}

void ASMGameState::RemovePlayerState(APlayerState* PlayerState)
{
	NET_LOG_CATEGORY(this, LogStereoMixGameState, Verbose, TEXT("RemovePlayerState: %s"), *PlayerState->GetName());
	Super::RemovePlayerState(PlayerState);
	if (ASMPlayerState* StereoMixPS = Cast<ASMPlayerState>(PlayerState))
	{
		OnPlayerRemovedEvent.Broadcast(StereoMixPS);
		K2_OnPlayerRemovedEvent.Broadcast(StereoMixPS);
	}
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ServerFPS, SharedParams);

	SharedParams.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsReadyToStart, SharedParams);
}

void ASMGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{
	Super::SeamlessTravelTransitionCheckpoint(bToTransitionMap);

	OnPlayerAddedEvent.Clear();
	OnPlayerRemovedEvent.Clear();
	K2_OnPlayerAddedEvent.Clear();
	K2_OnPlayerRemovedEvent.Clear();

	// Remove inactive and bots
	for (int32 i = PlayerArray.Num() - 1; i >= 0; i--)
	{
		APlayerState* PlayerState = PlayerArray[i];
		if (PlayerState && (PlayerState->IsABot() || PlayerState->IsInactive()))
		{
			RemovePlayerState(PlayerState);
		}
	}
}

UAbilitySystemComponent* ASMGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ASMGameState::ShouldShowLoadingScreen(FString& OutReason)
{
	if (!IsReadyToStart())
	{
		OutReason = TEXT("Game is not started.");
		return true;
	}
	return false;
}

USMAbilitySystemComponent* ASMGameState::GetStereoMixAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASMGameState::StartListeningGameReadyToStart(FSMOnGameReadyToStart::FDelegate&& Delegate)
{
	if (IsReadyToStart())
	{
		(void)Delegate.ExecuteIfBound();
		return;
	}
	OnGameReadyToStartEvent.Add(Delegate);
}

void ASMGameState::MulticastMessage_Implementation(const FSMVerbMessage Message)
{
	NET_LOG_CATEGORY(this, LogStereoMixGameState, Verbose, TEXT("MulticastMessage Received: %s"), *Message.ToString());
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* GameplayMessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			GameplayMessageSubsystem->BroadcastMessage(Message.Verb, Message);
		}
	}
}

void ASMGameState::MulticastReliableMessage_Implementation(const FSMVerbMessage Message)
{
	MulticastMessage_Implementation(Message);
}

void ASMGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ServerFPS, this);
	}
}

void ASMGameState::SetReadyToPlay()
{
	if (!bIsReadyToStart)
	{
		bIsReadyToStart = true;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsReadyToStart, this);
		OnRep_IsReadyToStart();
	}
}

void ASMGameState::OnRep_IsReadyToStart() const
{
	OnGameReadyToStartEvent.Broadcast();
	K2_OnGameReadyToStartEvent.Broadcast();
}
