// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SMActiveAbility.generated.h"

class USMActiveAbilityDisplaySlotWidget;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ESMActiveAbilityType : uint8
{
	None,
	Attack,
	Hold,
	Skill,
	NoiseBreak
};
