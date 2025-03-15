// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/SMPrimaryLayout.h"
#include "SMMenuLayout.generated.h"

class USMBackgroundScreen;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "StereoMix Menu Layout", Category = "StereoMix|UI")
class STEREOMIX_API USMMenuLayout : public USMPrimaryLayout
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:
	void InitBackground();
	void SetBackground(const TSubclassOf<USMBackgroundScreen>& Background) const;

	UPROPERTY(EditAnywhere, Category = "Layout")
	uint8 bShowBackground : 1 = false;

	UPROPERTY(EditAnywhere, Category = "Layout", meta = (EditCondition = "bShowBackground"))
	TSoftClassPtr<USMBackgroundScreen> BackgroundClass;
};
