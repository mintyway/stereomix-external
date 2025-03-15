// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "SMUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	USMUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);

	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;
};
