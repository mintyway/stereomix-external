// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingScreen.h"
#include "SMSettingScreen.generated.h"


class UInputAction;

UCLASS(Abstract, Category="Settings", meta=(DisableNativeTick))
class STEREOMIX_API USMSettingScreen : public UGameSettingScreen
{
	GENERATED_BODY()

public:
	USMSettingScreen();

protected:
	virtual void NativeOnInitialized() override;
	virtual UGameSettingRegistry* CreateRegistry() override;

	virtual bool NativeOnHandleBackAction() override;
	void HandleApplyAction();
	void HandleCancelChangesAction();

	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) override;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TObjectPtr<const UInputAction> ApplyInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TObjectPtr<const UInputAction> CancelChangesInputAction;

	UPROPERTY(EditAnywhere, Category="Settings")
	bool bDeactivateOnBackAction = true;

	FUIActionBindingHandle ApplyHandle;
	FUIActionBindingHandle CancelChangesHandle;
};
