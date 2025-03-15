// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "SMActivatableWidget.generated.h"

class USMButtonBase;
class UInputAction;
class USMActivatableWidgetStack;

UENUM(BlueprintType)
enum class ESMWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

USTRUCT(BlueprintType)
struct FVisibilityBindableWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	FWidgetChild Widget;

	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	ESlateVisibility ActivateVisibility = ESlateVisibility::HitTestInvisible;

	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	ESlateVisibility DeactivateVisibility = ESlateVisibility::Collapsed;
};

/**
 *
 */
UCLASS(Abstract, DisplayName = "StereoMix Activatable Widget", Category = "StereoMix|UI")
class STEREOMIX_API USMActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix Activatable Widget")
	void PerformBack();
	
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativePreConstruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;
	
	UFUNCTION()
	virtual void HandleVisibilityBindableWidgets();

	/** bUseCustomDeactivate가 true일 때 CustomDeactivate 로직을 처리하기 위해 호출하는 함수입니다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "StereoMix Activatable Widget", meta = (DisplayName = "On Custom Deactivate Requested"))
	void BP_OnCustomDeactivateRequested();
	virtual void NativeOnCustomDeactivateRequested();

	/** bUseCustomDeactivate가 true일 때 Deactivate를 완료하기 위해 호출하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StereoMix Activatable Widget")
	void FinishCustomDeactivate();

	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	ESMWidgetInputMode InputConfig = ESMWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	/** 마우스의 락 모드를 설정합니다. InputConfig가 Menu면 무조건 DoNotLock으로 설정됩니다. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseLockMode GameMouseLockMode = EMouseLockMode::LockOnCapture;
	
	/**
	 * true인 경우 바로 Deactivate 로직을 호출하지 않고 OnDeactivateRequested()를 호출하여 자식 위젯에서 로직을 처리할 수 있도록 합니다.
	 * 
	 * 자식 위젯에서는 반드시 로직 처리 후 PerformDeactivate()를 호출하여 Deactivate 로직을 수행해야 합니다.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation")
	bool bUseCustomDeactivate;

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	TArray<FVisibilityBindableWidget> VisibilityBindableWidgets;
};
