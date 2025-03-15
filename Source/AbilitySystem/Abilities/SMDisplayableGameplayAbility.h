// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMDisplayableGameplayAbility.generated.h"

/**
* StereoMix Gameplay Ability with displayable information
*/
UCLASS(Abstract, DisplayName = "StereoMix Displayable Gameplay Ability")
class STEREOMIX_API USMDisplayableGameplayAbility : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMDisplayableGameplayAbility();

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintPure, Category = "UI")
	UTexture2D* GetIcon() const { return Icon.LoadSynchronous(); }

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

private:
	int32 InputID = -1;
};
