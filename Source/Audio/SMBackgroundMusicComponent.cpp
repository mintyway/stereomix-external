// Copyright Studio Surround. All Rights Reserved.


#include "SMBackgroundMusicComponent.h"

#include "FMODEvent.h"
#include "Games/SMGameState.h"
#include "System/SMAssetManager.h"


USMBackgroundMusicComponent* USMBackgroundMusicComponent::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const ASMGameState* GameState = World->GetGameState<ASMGameState>())
		{
			return GameState->FindComponentByClass<USMBackgroundMusicComponent>();
		}
	}
	return nullptr;
}

void USMBackgroundMusicComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer)
	{
		if (BackgroundMusic.ToSoftObjectPath().IsValid())
		{
			FStreamableManager& StreamableManager = USMAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(BackgroundMusic.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnBackgroundMusicLoaded));
		}
	}
}

void USMBackgroundMusicComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MusicComponent)
	{
		MusicComponent->Stop();
		MusicComponent->DestroyComponent();
		MusicComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

bool USMBackgroundMusicComponent::IsPlaying() const
{
	return MusicComponent && MusicComponent->IsPlaying();
}

void USMBackgroundMusicComponent::Play()
{
	if (MusicComponent)
	{
		MusicComponent->Play();
	}
}

void USMBackgroundMusicComponent::Stop()
{
	if (MusicComponent)
	{
		MusicComponent->Stop();
	}
}

void USMBackgroundMusicComponent::OnBackgroundMusicLoaded()
{
	if (MusicComponent)
	{
		return;
	}

	MusicComponent = UFMODBlueprintStatics::PlayEventAttached(
		BackgroundMusic.Get(),
		GetOwner()->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		EAttachLocation::Type::KeepRelativeOffset,
		true, false, false);

	if (bStartAfterGameReadyToPlay)
	{
		if (ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>())
		{
			if (GameState->IsReadyToStart())
			{
				Play();
			}
			else
			{
				GameState->OnGameReadyToStartEvent.AddUObject(this, &ThisClass::Play);
			}
		}
	}
	else
	{
		Play();
	}
}
