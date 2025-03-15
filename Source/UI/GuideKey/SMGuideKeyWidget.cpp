// Copyright Studio Surround. All Rights Reserved.


#include "SMGuideKeyWidget.h"

#include "CommonActionWidget.h"
#include "InputAction.h"
#include "PlayerMappableKeySettings.h"
#include "StereoMixLog.h"
#include "Components/TextBlock.h"

void USMGuideKeyWidget::InitializeAction(const UInputAction* InAction)
{
	check(InAction);
	
	if (InputActionWidget)
	{
		InputActionWidget->SetEnhancedInputAction(ConstCast<const UInputAction>(InAction));
	}

	if (DisplayTextBlock)
	{
		if (TObjectPtr<UPlayerMappableKeySettings> KeySettings = InAction->GetPlayerMappableKeySettings())
		{
			DisplayTextBlock->SetText(KeySettings->DisplayName);
		}
		else
		{
			UE_LOG(LogStereoMixUI, Error, TEXT("No KeySettings found for action %s"), *GetNameSafe(InAction));
		}
	}
}
