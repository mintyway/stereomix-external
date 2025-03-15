// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FMODEvent.h"
#include "Input/CommonBoundActionButton.h"

#include "SMButtonBase.generated.h"

class UImage;
/**
 * StereoMix Common Button Base
 */
UCLASS(Abstract, DisplayName = "StereoMix Button Base", meta = (DisableNativeTick))
class STEREOMIX_API USMButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	USMButtonBase();

	UFUNCTION(BlueprintCallable, Category = "StereoMix Button|Setters")
	void SetButtonText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "StereoMix Button|Setters")
	void SetButtonIcon(const FSlateBrush& InIcon);

	UFUNCTION(BlueprintCallable, Category = "StereoMix Button|Setters")
	void SetButtonTexture(UTexture2D* InTexture);

	UFUNCTION(BlueprintCallable, Category = "StereoMix Button|Setters")
	void SetHoverAnimate(float InValue);

	UFUNCTION(BlueprintCallable, Category = "StereoMix Button|Setters")
	void SetDisableAnimate(float InValue);

	UFUNCTION(BlueprintPure, Category = "StereoMix Button|Getters")
	FText GetOverrideButtonText() const { return bOverrideButtonText ? ButtonText : FText::GetEmpty(); }

	UFUNCTION(BlueprintPure, Category = "StereoMix Button|Getters")
	bool ShouldUseIcon() const { return bUseIcon; }

	UFUNCTION(BlueprintPure, Category = "StereoMix Button|Getters")
	FSlateBrush GetButtonIconBrush() const { return IconBrush; }

	UFUNCTION(BlueprintPure, Category = "StereoMix Button|Getters")
	UTexture2D* GetButtonTexture() const { return ButtonTexture; }

protected:
	virtual void NativePreConstruct() override;

	virtual void UpdateInputActionWidget() override;

	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;

	virtual void NativeOnSelected(bool bBroadcast) override;

	virtual void NativeOnPressed() override;

	virtual void NativeOnClicked() override;

	virtual void NativeOnEnabled() override;

	virtual void NativeOnDisabled() override;

	virtual void HandleTriggeringActionCommited() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "StereoMix Button", meta = (DisplayName = "On Button Style Changed"))
	void BP_OnButtonStyleChanged();
	void NativeOnButtonStyleChanged();

	UFUNCTION(BlueprintCallable)
	void UpdateButtonTextureToButtonMID();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "StereoMix Button|Getters")
	UMaterialInstanceDynamic* GetButtonBorderMID() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StereoMix Button|Animations")
	float HoverAnimate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (ExposeOnSpawn))
	uint8 bHideInputActionOnUnhovered : 1 = false;
	

private:
	UPROPERTY(EditAnywhere, Category = "StereoMix Button|Sounds", DisplayName = "Hovered Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> HoveredFMODSoundOverride;

	UPROPERTY(EditAnywhere, Category = "StereoMix Button|Sounds", DisplayName = "Selected Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> SelectedFMODSoundOverride;

	UPROPERTY(EditAnywhere, Category = "StereoMix Button|Sounds", DisplayName = "Pressed Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> PressedFMODSoundOverride;

	UPROPERTY(EditAnywhere, Category = "StereoMix Button|Sounds", DisplayName = "Clicked Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> ClickedFMODSoundOverride;

	UPROPERTY(EditAnywhere, Category="StereoMix Button|Sounds", meta=(InlineEditConditionToggle))
	uint8 bOverrideButtonText : 1 = false;

	UPROPERTY(EditAnywhere, Category="StereoMix Button", meta=(EditCondition="bOverrideButtonText"))
	FText ButtonText;

	UPROPERTY(EditAnywhere, Category="StereoMix Button", meta=(InlineEditConditionToggle))
	uint8 bUseIcon : 1 = false;

	UPROPERTY(EditAnywhere, Category="StereoMix Button", meta=(EditCondition="bUseIcon"))
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, Category="StereoMix Button", meta=(InlineEditConditionToggle))
	uint8 bUseButtonTexture : 1 = false;

	UPROPERTY(EditAnywhere, Category="StereoMix Button", meta=(EditCondition="bUseButtonTexture"))
	TObjectPtr<UTexture2D> ButtonTexture;

	UPROPERTY(Transient)
	TObjectPtr<const UInputAction> CachedInputAction;
	
	FTimerHandle ButtonStyleChangeHandle;
};
