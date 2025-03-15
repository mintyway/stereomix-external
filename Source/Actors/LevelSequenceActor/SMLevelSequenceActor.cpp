// Copyright Studio Surround. All Rights Reserved.


#include "SMLevelSequenceActor.h"

#include "EngineUtils.h"
#include "LevelSequencePlayer.h"


ASMLevelSequenceActor::ASMLevelSequenceActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicatePlayback = true;
}

void ASMLevelSequenceActor::PlaySequence()
{
	if (ULevelSequencePlayer* CachedSequencePlayer = GetSequencePlayer())
	{
		FMovieSceneSequencePlaybackSettings LevelSequencePlaybackSettings;
		LevelSequencePlaybackSettings.bAutoPlay = true;
		CachedSequencePlayer->SetPlaybackSettings(LevelSequencePlaybackSettings);
		CachedSequencePlayer->OnNativeFinished.BindUObject(this, &ThisClass::LevelSequenceFinishedCallback);
		CachedSequencePlayer->Play();
	}
}

void ASMLevelSequenceActor::LevelSequenceFinishedCallback() const
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		PlayerController->ClientSetViewTarget(PlayerController->GetPawn(), FViewTargetTransitionParams());
	}
	OnLevelSequenceFinishedEvent.Broadcast();
}
