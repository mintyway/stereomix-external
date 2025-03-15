// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSpawnManagerComponent.h"

#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/PlayerStart/SMPlayerStartBase.h"
#include "Games/SMGameMode.h"
#include "Player/SMPlayerController.h"
#include "Utilities/SMLog.h"


USMCharacterSpawnManagerComponent::USMCharacterSpawnManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(false);
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USMCharacterSpawnManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// if (GetOwner()->GetClass() != ASMGameMode::StaticClass())
	// {
	// 	UE_LOG(LogStereoMix, Error, TEXT("USMCharacterSpawnManagerComponent는 게임모드 전용 컴포넌트입니다."));
	// 	return;
	// }

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<ASMPlayerStartBase> It(World); It; ++It)
	{
		if (ASMPlayerStartBase* PlayerStart = *It)
		{
			CachedPlayerStarts.Add(PlayerStart);
		}
	}
}

TSubclassOf<ASMPlayerCharacterBase> USMCharacterSpawnManagerComponent::FindCharacterClassForPlayer(const ASMPlayerController* TargetController) const
{
	if (TargetController)
	{
		const ESMCharacterType CharacterType = TargetController->GetCharacterType();
		const ESMTeam Team = TargetController->GetTeam();

		if (CharacterType == ESMCharacterType::None)
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMix, Error, TEXT("FindCharacterClassForPlayer: %s: CharacterType is none."), *TargetController->GetName());
			return nullptr;
		}
		if (Team == ESMTeam::None)
		{
			NET_LOG_CATEGORY(GetOwner(), LogStereoMix, Error, TEXT("FindCharacterClassForPlayer: %s: Team is none."), *TargetController->GetName());
			return nullptr;
		}

		TSoftObjectPtr<USMPlayerCharacterDataAsset> DataPtr = CharacterSpawnData.FindRef(CharacterType);
		USMPlayerCharacterDataAsset* CharacterData = DataPtr.LoadSynchronous();
		check(CharacterData);

		TSubclassOf<ASMPlayerCharacterBase> CharacterClass = CharacterData->CharacterClass[Team];
		check(CharacterClass);
		return CharacterClass;
	}

	return nullptr;
}

AActor* USMCharacterSpawnManagerComponent::ChoosePlayerStart(AController* Player) const
{
	if (Player)
	{
#if WITH_EDITOR
		if (APlayerStart* PlayerStart = FindPlayFromHereStart(Player))
		{
			return PlayerStart;
		}
#endif

		if (ASMPlayerController* StereoMixPC = Cast<ASMPlayerController>(Player))
		{
			const ESMCharacterType CharacterType = StereoMixPC->GetCharacterType();
			const ESMTeam Team = StereoMixPC->GetTeam();
			const FString StarterTag = ASMPlayerStartBase::GetPlayerStartTagByTypeAndTeam(CharacterType, Team);
			for (TWeakObjectPtr<ASMPlayerStartBase> TargetPlayerStart : CachedPlayerStarts)
			{
				if (TargetPlayerStart.IsValid() && TargetPlayerStart->PlayerStartTag == StarterTag)
				{
					return TargetPlayerStart.Get();
				}
			}

			if (CachedPlayerStarts.Num() > 0)
			{
				NET_LOG(Player, Warning, TEXT("No player start found with tag %s. Choosing a first start actor."), *StarterTag);
				return CachedPlayerStarts[0].Get();
			}
		}
	}

	return nullptr;
}

void USMCharacterSpawnManagerComponent::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld == GetWorld())
	{
		for (AActor* Actor : InLevel->Actors)
		{
			if (ASMPlayerStartBase* PlayerStart = Cast<ASMPlayerStartBase>(Actor))
			{
				ensure(!CachedPlayerStarts.Contains(PlayerStart));
				CachedPlayerStarts.Add(PlayerStart);
			}
		}
	}
}

void USMCharacterSpawnManagerComponent::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (ASMPlayerStartBase* PlayerStart = Cast<ASMPlayerStartBase>(SpawnedActor))
	{
		CachedPlayerStarts.Add(PlayerStart);
	}
}

#if WITH_EDITOR
APlayerStart* USMCharacterSpawnManagerComponent::FindPlayFromHereStart(const AController* Player) const
{
	// Only 'Play From Here' for a player controller, bots etc. should all spawn from normal spawn points.
	if (Player->IsA<APlayerController>())
	{
		if (UWorld* World = GetWorld())
		{
			for (TActorIterator<APlayerStart> It(World); It; ++It)
			{
				if (APlayerStart* PlayerStart = *It)
				{
					if (PlayerStart->IsA<APlayerStartPIE>())
					{
						// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
						return PlayerStart;
					}
				}
			}
		}
	}

	return nullptr;
}
#endif
