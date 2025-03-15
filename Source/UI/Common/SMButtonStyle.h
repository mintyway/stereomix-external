// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FMODEvent.h"

#include "SMButtonStyle.generated.h"

/**
 *
 */
UCLASS(Abstract, DisplayName = "StereoMix Button Style")
class STEREOMIX_API USMButtonStyle : public UCommonButtonStyle
{
	GENERATED_BODY()

public:
	// =============================================================================
	// Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess), DisplayName = "Hovered Sound (FMOD)")
	TObjectPtr<UFMODEvent> HoveredFMODSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess), DisplayName = "Selected Sound (FMOD)")
	TObjectPtr<UFMODEvent> SelectedFMODSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess), DisplayName = "Pressed Sound (FMOD)")
	TObjectPtr<UFMODEvent> PressedFMODSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess), DisplayName = "Clicked Sound (FMOD)")
	TObjectPtr<UFMODEvent> ClickedFMODSound;
};
