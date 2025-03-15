// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/SMUserWidget.h"

#include "SMNameplateWidget.generated.h"

class USMAbilitySystemComponent;
class ASMCharacterBase;
class ASMPlayerCharacterBase;
class USMCharacterAttributeSet;
class ASMPlayerState;
class UProgressBar;
class UTextBlock;


UCLASS(Abstract, DisplayName = "StereoMix Nameplate Widget")
class STEREOMIX_API USMNameplateWidget : public USMUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Nameplate", DisplayName = "On Register Character")
	void BP_RegisterCharacter(ASMCharacterBase* InOwningCharacter);
	void RegisterCharacter(ASMCharacterBase* InOwningCharacter);

	UFUNCTION(BlueprintPure, Category = "Nameplate")
	ASMCharacterBase* GetOwningCharacter() const;

	UFUNCTION(BlueprintPure, Category = "Nameplate")
	ASMPlayerCharacterBase* GetOwningPlayerCharacter() const;

	UFUNCTION(BlueprintPure, Category = "Nameplate")
	const USMCharacterAttributeSet* GetOwningCharacterAttributeSet() const;

	UFUNCTION(BlueprintPure, Category = "Nameplate")
	USMAbilitySystemComponent* GetOwningAbilitySystemComponent() const;

private:
	TObjectPtr<ASMCharacterBase> OwningCharacter;
};
