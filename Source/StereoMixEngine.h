// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "Engine/Engine.h"
#include "Engine/GameEngine.h"
#include "StereoMixEngine.generated.h"

class IEngineLoop;
class UObject;

UCLASS()
class UStereoMixEngine : public UGameEngine
{
	GENERATED_BODY()

public:
	UStereoMixEngine(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
