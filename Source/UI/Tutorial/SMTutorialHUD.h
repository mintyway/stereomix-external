// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Game/SMGameHUDLayout.h"
#include "SMTutorialHUD.generated.h"

class USMTutorialKeyInfo;
class USMTutorialMission;
class USMTutorialSuccess;
class USMTutorialGuide;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMTutorialHUD : public USMGameHUDLayout
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialGuide> TutorialGuide;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialSuccess> TutorialSuccess;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialMission> TutorialMission;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialKeyInfo> TutorialKeyInfo;
};
