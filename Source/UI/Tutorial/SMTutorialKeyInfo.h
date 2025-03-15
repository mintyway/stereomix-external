// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMTutorialKeyInfo.generated.h"

DECLARE_DELEGATE_OneParam(OnTestDelegate, int32);

class UOverlay;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMTutorialKeyInfo : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	USMTutorialKeyInfo();

	virtual void NativeConstruct() override;

	void HideAll();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowLeftClick();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "KeyInfo")
	bool HideLeftClick();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowRightClick();

	UFUNCTION(BlueprintImplementableEvent, Category = "KeyInfo")
	bool HideRightClick();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowRightLeftClick(bool bUseDimRightClick, bool bUseDimLeftClick);

	UFUNCTION(BlueprintImplementableEvent, Category = "KeyInfo")
	bool HideRightLeftClick();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowWASDKey();

	UFUNCTION(BlueprintImplementableEvent, Category = "KeyInfo")
	bool HideWASDKey();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowEKey();

	UFUNCTION(BlueprintImplementableEvent, Category = "KeyInfo")
	bool HideEKey();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> LeftClickOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> RightClickOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> RightLeftClickOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> WASDOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> EOverlay;

protected:
	UFUNCTION(BlueprintCallable)
	void InternalHideLeftClick() const;

	UFUNCTION(BlueprintCallable)
	void InternalHideRightClick() const;

	UFUNCTION(BlueprintCallable)
	void InternalHideRightLeftClick() const;

	UFUNCTION(BlueprintCallable)
	void InternalHideWASDKey() const;

	UFUNCTION(BlueprintCallable)
	void InternalHideEKey() const;

	UPROPERTY()
	TArray<TObjectPtr<UWidget>> Overlays;
};
