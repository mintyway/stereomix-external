// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Iris/Core/IrisProfiler.h"

#include "SMGameHUD.generated.h"

class USMPerfStatContainer;
class USMGameVersionWidget;
/**
 *
 */
UCLASS()
class STEREOMIX_API ASMGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASMGameHUD();

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitPlayerState(APlayerState* PlayerState);

	APlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

private:
	UPROPERTY(Transient)
	TObjectPtr<APlayerState> OwningPlayerState;

	UPROPERTY(Transient)
	TObjectPtr<USMGameVersionWidget> GameVersionWidget;

	UPROPERTY(Transient)
	TObjectPtr<USMPerfStatContainer> PerformanceStatContainerWidget;
};
