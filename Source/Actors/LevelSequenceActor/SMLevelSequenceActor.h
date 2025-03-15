// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequenceActor.h"
#include "SMLevelSequenceActor.generated.h"

DECLARE_MULTICAST_DELEGATE(FSMOnLevelSequenceFinished);

UCLASS(DisplayName = "StereoMix Level Sequence Actor")
class STEREOMIX_API ASMLevelSequenceActor : public ALevelSequenceActor
{
	GENERATED_BODY()

public:
	ASMLevelSequenceActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "StereoMix|LevelSequence")
	void PlaySequence();

	FSMOnLevelSequenceFinished OnLevelSequenceFinishedEvent;

private:
	void LevelSequenceFinishedCallback() const;
};
