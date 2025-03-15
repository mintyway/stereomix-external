// Copyright Studio Surround. All Rights Reserved.


#include "SMGameHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Development/SMDebugWidgetSettings.h"

ASMGameHUD::ASMGameHUD()
{
}

void ASMGameHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASMGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (const USMDebugWidgetSettings* DebugWidgetSettings = GetDefault<USMDebugWidgetSettings>())
	{
		if (TSubclassOf<USMGameVersionWidget> GameVersionWidgetClass = DebugWidgetSettings->GameVersionWidgetClass.LoadSynchronous())
		{
			GameVersionWidget = CreateWidget<USMGameVersionWidget>(GetOwningPlayerController(), GameVersionWidgetClass);
			if (GameVersionWidget)
			{
				GameVersionWidget->AddToViewport();
			}
		}

		TSubclassOf<USMPerfStatContainer> PerfStatWidgetClass;
		if (GetNetMode() == NM_Standalone)
		{
			PerfStatWidgetClass = DebugWidgetSettings->StandalonePerformanceStatContainerClass.LoadSynchronous();
		}
		else
		{
			PerfStatWidgetClass = DebugWidgetSettings->NetworkPerformanceStatContainerClass.LoadSynchronous();
		}
		if (PerfStatWidgetClass)
		{
			PerformanceStatContainerWidget = CreateWidget<USMPerfStatContainer>(GetOwningPlayerController(), PerfStatWidgetClass);
			if (PerformanceStatContainerWidget)
			{
				PerformanceStatContainerWidget->AddToViewport(100);
			}
		}
	}
}

void ASMGameHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASMGameHUD::InitPlayerState(APlayerState* PlayerState)
{
	OwningPlayerState = PlayerState;
}
