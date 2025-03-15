// Copyright Studio Surround. All Rights Reserved.


#include "SMButtonBase.h"

#include "CommonActionWidget.h"
#include "FMODBlueprintStatics.h"
#include "SMButtonStyle.h"
#include "Animation/UMGSequencePlayer.h"

#define BUTTON_PLAY_FMOD_SOUND(SourceVariableName) \
if (SourceVariableName##Override)\
{\
	UFMODBlueprintStatics::PlayEvent2D(this, SourceVariableName##Override, true);\
}\
else if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->SourceVariableName)\
{\
	UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->SourceVariableName, true);\
}

USMButtonBase::USMButtonBase()
{
	bHideInputActionWithKeyboard = true;
	bDisplayInputActionWhenNotInteractable = false;
}

void USMButtonBase::SetButtonText(const FText& InText)
{
	bOverrideButtonText = !InText.IsEmpty();
	ButtonText = InText;
	NativeOnButtonStyleChanged();
}

void USMButtonBase::SetButtonIcon(const FSlateBrush& InIcon)
{
	bUseIcon = InIcon.IsSet();
	IconBrush = InIcon;
	
	NativeOnButtonStyleChanged();
}

void USMButtonBase::SetButtonTexture(UTexture2D* InTexture)
{
	ButtonTexture = InTexture;
	NativeOnButtonStyleChanged();
}

void USMButtonBase::SetHoverAnimate(float InValue)
{
	static const FName ButtonHoveredMaterialParamName = FName("IsHovered");
		
	HoverAnimate = InValue;
	if (UMaterialInstanceDynamic* ButtonMID = GetButtonBorderMID())
	{
		ButtonMID->SetScalarParameterValue(ButtonHoveredMaterialParamName, FMath::Clamp(HoverAnimate, 0.0f, 1.0f));
	}
}

void USMButtonBase::SetDisableAnimate(float InValue)
{
	static const FName ButtonDisabledMaterialParamName = FName("IsDisabled");

	if (UMaterialInstanceDynamic* ButtonMID = GetButtonBorderMID())
	{
		ButtonMID->SetScalarParameterValue(ButtonDisabledMaterialParamName, InValue);
	}
}

void USMButtonBase::NativePreConstruct()
{
	if (!bUseIcon)
	{
		IconBrush = FSlateBrush(FSlateOptionalBrush());
	}
	Super::NativePreConstruct();
}

void USMButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();
	if (bHideInputActionOnUnhovered)
	{
		SetHideInputAction(!IsHovered());
	}
	NativeOnButtonStyleChanged();
}

void USMButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);
	NativeOnButtonStyleChanged();
}

void USMButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();
	BUTTON_PLAY_FMOD_SOUND(HoveredFMODSound)
}

void USMButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
}

void USMButtonBase::NativeOnSelected(bool bBroadcast)
{
	Super::NativeOnSelected(bBroadcast);
	BUTTON_PLAY_FMOD_SOUND(SelectedFMODSound)
}

void USMButtonBase::NativeOnPressed()
{
	Super::NativeOnPressed();
	if (!GetLocked())
	{
		BUTTON_PLAY_FMOD_SOUND(PressedFMODSound)
	}
}

void USMButtonBase::NativeOnClicked()
{
	Super::NativeOnClicked();
	BUTTON_PLAY_FMOD_SOUND(ClickedFMODSound)
}

void USMButtonBase::NativeOnEnabled()
{
	Super::NativeOnEnabled();
}

void USMButtonBase::NativeOnDisabled()
{
	Super::NativeOnDisabled();
}

void USMButtonBase::HandleTriggeringActionCommited()
{
	Super::HandleTriggeringActionCommited();
}

void USMButtonBase::NativeOnButtonStyleChanged()
{
	if (GetWorld())
	{
		if (!ButtonStyleChangeHandle.IsValid())
		{
			ButtonStyleChangeHandle = GetWorld()->GetTimerManager().SetTimerForNextTick([this] {
				BP_OnButtonStyleChanged();
				ButtonStyleChangeHandle.Invalidate();
			});
		}
	}
	else
	{
		BP_OnButtonStyleChanged();
	}
}

void USMButtonBase::UpdateButtonTextureToButtonMID()
{
	static const FName ButtonTextureParamName(TEXT("SourceTexture"));

	// 여기서 텍스처를 바꾸더라도 버튼 머티리얼이 UseTexture==false인 경우 바뀌지 않음 
	if (ButtonTexture)
	{
		if (UMaterialInstanceDynamic* ButtonMID = GetButtonBorderMID())
		{
			ButtonMID->SetTextureParameterValue(ButtonTextureParamName, ButtonTexture);
		}
	}
}

UMaterialInstanceDynamic* USMButtonBase::GetButtonBorderMID_Implementation() const
{
	return GetSingleMaterialStyleMID();
}