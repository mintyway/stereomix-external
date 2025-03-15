// Copyright Studio Surround. All Rights Reserved.


#include "SMGameHUDLayout.h"

#include "CommonUIExtensions.h"
#include "SMGameplayTags.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputTypes.h"

using namespace StereoMix;

void USMGameHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (EscapeAction)
	{
		FBindUIActionArgs Args(EscapeAction, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction));
		Args.InputMode = ECommonInputMode::Game;
		RegisterUIActionBinding(Args);
	}
}

void USMGameHUDLayout::NativeConstruct()
{
	Super::NativeConstruct();

	// GameHUD는 포커싱이 없어서 명시적으로 InputConfig를 설정해줘야 합니다.
	UCommonUIActionRouterBase* ActionRouter = UCommonUIActionRouterBase::Get(*this);
	ActionRouter->SetActiveUIInputConfig(GetDesiredInputConfig().Get(FUIInputConfig()), this);
}

TOptional<FUIInputConfig> USMGameHUDLayout::GetDesiredInputConfig() const
{
	TOptional<FUIInputConfig> NewInputConfig = Super::GetDesiredInputConfig();

	// 마우스 락 모드를 항상 설정합니다.
	NewInputConfig = FUIInputConfig(
		ECommonInputMode::Game,
		EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown,
		EMouseLockMode::LockAlways,
		false);
	return NewInputConfig;
}

void USMGameHUDLayout::HandleEscapeAction() const
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), Tags::UI_Layer_GameMenu, EscapeMenuClass);
	}
}
