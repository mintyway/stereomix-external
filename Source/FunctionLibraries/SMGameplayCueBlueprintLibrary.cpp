// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayCueBlueprintLibrary.h"

#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"

USceneComponent* USMGameplayCueBlueprintLibrary::GetTargetComponent(const FGameplayCueParameters& Parameters)
{
	return Parameters.TargetAttachComponent.Get();
}

void USMGameplayCueBlueprintLibrary::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation)
{
	OutLocation = Parameters.Location;

	OutRotation = FRotator::ZeroRotator;
	if (!Parameters.Normal.IsNearlyZero())
	{
		OutRotation = Parameters.Normal.Rotation();
	}
}

bool USMGameplayCueBlueprintLibrary::GetIntegerFromAttackTag(FGameplayTag AttackTag, int32& OutInteger)
{
	// Attack 태그 하위의 태그들을 받으면 상위 태그의 비교를 통해 정수값을 반환합니다.
	if (AttackTag.MatchesTag(SMTags::Ability::Attack))
	{
		OutInteger = 1;
		return true;
	}
	if (AttackTag.MatchesTag(SMTags::Ability::Skill))
	{
		OutInteger = 2;
		return true;
	}
	if (AttackTag.MatchesTag(SMTags::Ability::NoiseBreak))
	{
		OutInteger = 3;
		return true;
	}

	return false;
}

bool USMGameplayCueBlueprintLibrary::GetAttackTagFromInteger(int32 Integer, FGameplayTag& OutAttackTag)
{
	// 여기서 상위 태그를 반환하면 태그 비교 시 PartialMatch 옵션을 사용해야 합니다
	switch (Integer)
	{
		case 1:
			OutAttackTag = SMTags::Ability::Attack;
			return true;
		case 2:
			OutAttackTag = SMTags::Ability::Skill;
			return true;
		case 3:
			OutAttackTag = SMTags::Ability::NoiseBreak;
			return true;
		default:
			return false;
	}
}
