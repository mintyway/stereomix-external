// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Utilities/SMLog.h"

USMCharacterAttributeSet::USMCharacterAttributeSet()
{
	Damage = 0.0f;

	MaxPostureGauge = 100.0f;
	InitPostureGauge(GetMaxPostureGauge());

	MoveSpeed = 0.0f;

	InitMaxStamina(100.0f);
	InitStamina(GetMaxStamina());

	InitMaxSkillGauge(100.0f);
	InitSkillGauge(GetMaxSkillGauge());

	InvincibleStateTags.AddTag(SMTags::Character::State::Common::Invincible);

	UnlimitStaminaTags.AddTag(SMTags::Character::State::Common::UnlimitStamina);
}

void USMCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, PostureGauge, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, MaxPostureGauge, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, MoveSpeed, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, Stamina, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, MaxStamina, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, SkillGauge, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(USMCharacterAttributeSet, MaxSkillGauge, SharedParams);

	SharedParams.Condition = COND_OwnerOnly;
	// Owner 전용 필드는 여기다가 추가
}

void USMCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}

	if (Attribute == GetHealAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}

	if (Attribute == GetHealSkillGaugeAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}

	if (Attribute == GetHealStaminaAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}
}

void USMCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetPostureGaugeAttribute())
	{
		if (GetPostureGauge() <= 0.0f)
		{
			const UAbilitySystemComponent* SourceASC = GetOwningAbilitySystemComponent();
			if (ensure(SourceASC))
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceASC->GetAvatarActor(), SMTags::Event::Character::Neutralize, FGameplayEventData());
			}
		}
	}

	if (Attribute == GetStaminaAttribute())
	{
		if (GetStamina() <= 0.0f)
		{
			if (UAbilitySystemComponent* SourceASC = GetOwningAbilitySystemComponent())
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceASC->GetAvatarActor(), SMTags::Event::Character::Exhausted, FGameplayEventData());
			}
		}
	}
}

bool USMCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.Target.HasAnyMatchingGameplayTags(InvincibleStateTags))
		{
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		if (Data.Target.HasAnyMatchingGameplayTags(UnlimitStaminaTags))
		{
			Data.EvaluatedData.Magnitude = Data.EvaluatedData.Magnitude < 0.0f ? 0.0f : Data.EvaluatedData.Magnitude;
			return false;
		}
	}

	if (Data.EvaluatedData.Attribute == GetSkillGaugeAttribute())
	{
		if (Data.Target.HasMatchingGameplayTag(SMTags::Character::State::Common::UnlimitSkillGauge))
		{
			Data.EvaluatedData.Magnitude = Data.EvaluatedData.Magnitude > 0.0f ? Data.EvaluatedData.Magnitude : 0.0f;
			return false;
		}
	}

	return true;
}

void USMCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const AActor* OwnerActor = Data.Target.GetAvatarActor();
	if (!OwnerActor)
	{
		return;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float NewPostureGauge = FMath::Clamp(GetPostureGauge() - GetDamage(), 0.0f, GetMaxPostureGauge());
		SetPostureGauge(NewPostureGauge);
		SetDamage(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetHealAttribute())
	{
		const float NewPostureGauge = FMath::Clamp(GetPostureGauge() + GetHeal(), 0.0f, GetMaxPostureGauge());
		SetPostureGauge(NewPostureGauge);
		SetHeal(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetHealSkillGaugeAttribute())
	{
		const float NewSkillGauge = FMath::Clamp(GetSkillGauge() + GetHealSkillGauge(), 0.0f, GetMaxSkillGauge());
		SetSkillGauge(NewSkillGauge);
		SetHealSkillGauge(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetHealStaminaAttribute())
	{
		const float NewStamina = FMath::Clamp(GetStamina() + GetHealStamina(), 0.0f, GetMaxStamina());
		SetStamina(NewStamina);
		SetHealStamina(0.0f);
	}
}

void USMCharacterAttributeSet::OnRep_PostureGauge(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, PostureGauge, OldValue);
}

void USMCharacterAttributeSet::OnRep_MaxPostureGauge(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MaxPostureGauge, OldValue);
}

void USMCharacterAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MoveSpeed, OldValue);
}

void USMCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, Stamina, OldValue);
}

void USMCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MaxStamina, OldValue);
}

void USMCharacterAttributeSet::OnRep_SkillGauge(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, SkillGauge, OldValue);
}

void USMCharacterAttributeSet::OnRep_MaxSkillGauge(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MaxSkillGauge, OldValue);
}
