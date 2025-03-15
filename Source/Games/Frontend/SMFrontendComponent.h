// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ControlFlowNode.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "Flow/SMFlowComponent.h"
#include "SMFrontendComponent.generated.h"


enum class ECommonUserOnlineContext : uint8;
enum class ECommonUserPrivilege : uint8;
class UCommonUserInfo;
class USMMenuLayout;
class UCommonActivatableWidget;

UCLASS(Abstract, DisplayName = "StereoMix Frontend Flow Component")
class STEREOMIX_API USMFrontendComponent : public USMFlowComponent
{
	GENERATED_BODY()

public:
	USMFrontendComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void MakeFlow(FControlFlow& Flow) override;

private:
	void FlowStep_TryShowSplashScreen(FControlFlowNodeRef SubFlow);
	void FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow);
	void FlowStep_WaitForUserLogin(FControlFlowNodeRef SubFlow);
	void FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow);
	void FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow);

	UPROPERTY(EditAnywhere, Category = UI)
	TSoftClassPtr<UCommonActivatableWidget> SplashScreenClass;

	UPROPERTY(EditAnywhere, Category = UI)
	TSoftClassPtr<USMMenuLayout> StartScreenClass;

	UPROPERTY(EditAnywhere, Category = UI)
	TSoftClassPtr<USMMenuLayout> MainScreenClass;

	UPROPERTY(EditAnywhere, Category = UI)
	TSoftClassPtr<USMMenuLayout> LobbyScreenClass;

	UPROPERTY(EditAnywhere, Category = Sound)
	TSoftObjectPtr<UFMODEvent> BackgroundMusic;

	TSharedPtr<FControlFlow> FrontEndFlow;
	FFMODEventInstance BackgroundMusicInstance{};
	FDelegateHandle OnJoinSessionCompleteEventHandle;
};
