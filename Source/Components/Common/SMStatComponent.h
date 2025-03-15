// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "SMStatComponent.generated.h"


UCLASS(DisplayName = "StereoMix Stat Component", meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMStatComponent : public UPlayerStateComponent
{
	GENERATED_BODY()

public:
	USMStatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void CopyProperties(UPlayerStateComponent* TargetPlayerStateComponent) override;

protected:
	
};
