// Copyright Studio Surround. All Rights Reserved.


#include "SMActiveAbilityDisplayEntryWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SMActiveAbilityDisplaySlotWidget.h"
#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SMDisplayableGameplayAbility.h"


USMActiveAbilityDisplayEntryWidget::USMActiveAbilityDisplayEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EntryData = FSMActiveAbilityDisplayEntryData();
	CurrentState = ESMAbilityDisplayState::None;
}

void USMActiveAbilityDisplayEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMActiveAbilityDisplayEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void USMActiveAbilityDisplayEntryWidget::SetEntryData(USMActiveAbilityDisplaySlotWidget* InEntrySlot, const FSMActiveAbilityDisplayEntryData& InEntryData)
{
	EntrySlot = InEntrySlot;
	EntryData = InEntryData;
	BP_OnEntryInitialized();
}

bool USMActiveAbilityDisplayEntryWidget::TryGetGauge(float& OutCurrentGauge, float& OutMaxGauge) const
{
	if (!EntrySlot || !HasGauge())
	{
		return false;
	}

	USMAbilitySystemComponent* ASC = EntrySlot->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	OutCurrentGauge = ASC->GetNumericAttribute(EntryData.GaugeAttribute);
	OutMaxGauge = ASC->GetNumericAttribute(EntryData.MaxGaugeAttribute);
	return true;
}

bool USMActiveAbilityDisplayEntryWidget::HasGauge() const
{
	return EntryData.bHasGauge && EntryData.GaugeAttribute.IsValid() && EntryData.MaxGaugeAttribute.IsValid();
}

const FSMActiveAbilityDisplayEntryData& USMActiveAbilityDisplayEntryWidget::GetEntryData() const
{
	return EntryData;
}

ESMActiveAbilityType USMActiveAbilityDisplayEntryWidget::GetAbilityType() const
{
	return EntryData.ActiveType;
}

USMDisplayableGameplayAbility* USMActiveAbilityDisplayEntryWidget::RefreshAbilityInstance()
{
	if (USMAbilitySystemComponent* ASC = EntrySlot->GetAbilitySystemComponent())
	{
		if (FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(EntryData.GetInputID()))
		{
			if (USMDisplayableGameplayAbility* GA = Cast<USMDisplayableGameplayAbility>(GASpec->GetPrimaryInstance()))
			{
				if (GA != AbilityInstance)
				{
					AbilityInstance = GA;
					BP_OnAbilityUpdated(AbilityInstance);
				}
				// UE_LOG(LogTemp, Warning, TEXT("GetGA: %s, InputID: %d"), *GA->GetName(), EntryData.GetInputID());
				return AbilityInstance;
			}
		}
	}
	return nullptr;
}

USMDisplayableGameplayAbility* USMActiveAbilityDisplayEntryWidget::GetCachedAbilityInstance() const
{
	return AbilityInstance;
}

ESMAbilityDisplayState USMActiveAbilityDisplayEntryWidget::GetCurrentState() const
{
	return CurrentState;
}

void USMActiveAbilityDisplayEntryWidget::SetState(ESMAbilityDisplayState NewState)
{
	if (NewState != CurrentState)
	{
		const ESMAbilityDisplayState OldState = CurrentState;
		CurrentState = NewState;
		if (const USMDisplayableGameplayAbility* GA = RefreshAbilityInstance())
		{
			// UE_LOG(LogStereoMixUI, Warning, TEXT("<%s> SetState: %s -> %s"), *GA->GetName(), *UEnum::GetValueAsString(OldState), *UEnum::GetValueAsString(CurrentState));
			HandleNewState(OldState, NewState);
			BP_OnUpdateState(OldState, NewState);
		}
	}
}

bool USMActiveAbilityDisplayEntryWidget::TryUpdateState(ESMAbilityDisplayState& OutOldState, ESMAbilityDisplayState& OutNewState)
{
	USMAbilitySystemComponent* ASC = EntrySlot->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	ESMAbilityDisplayState NextState;
	if (const USMDisplayableGameplayAbility* GA = RefreshAbilityInstance())
	{
		const FGameplayAbilitySpecHandle Handle = GA->GetCurrentAbilitySpecHandle();
		const FGameplayAbilityActorInfo& ActorInfo = GA->GetActorInfo();

		float CurrentGauge, MaxGauge;
		bool bHasGauge = TryGetGauge(CurrentGauge, MaxGauge);

		if (GA->IsActive())
		{
			NextState = ESMAbilityDisplayState::Activated;
		}
		else if (!GA->CanActivateAbility(Handle, &ActorInfo))
		{
			if (bHasGauge && CurrentGauge != MaxGauge)
			{
				NextState = ESMAbilityDisplayState::Cooldown;
				BP_OnUpdateGauge(CurrentGauge, MaxGauge);
			}
			else
			{
				if (EntryData.bHideOnDeactivate)
				{
					NextState = ESMAbilityDisplayState::Hidden;
				}
				else
				{
					NextState = ESMAbilityDisplayState::Deactivated;
				}
			}
		}
		else
		{
			NextState = ESMAbilityDisplayState::Idle;
		}
		// UE_LOG(LogTemp, Log, TEXT("GA: %s (%s)"), *GA->GetName(), *UEnum::GetValueAsString(NextState));
	}
	else
	{
		// UE_LOG(LogStereoMixUI, Warning, TEXT("Ability instance is not valid."));
		NextState = ESMAbilityDisplayState::Hidden;
	}

	if (CurrentState == NextState)
	{
		return false;
	}

	OutOldState = CurrentState;
	OutNewState = NextState;
	SetState(NextState);
	return true;
}

void USMActiveAbilityDisplayEntryWidget::HandleNewState(const ESMAbilityDisplayState& OldState, const ESMAbilityDisplayState& NewState)
{
	switch (NewState)
	{
		case ESMAbilityDisplayState::Idle:
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case ESMAbilityDisplayState::Activated:
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case ESMAbilityDisplayState::Deactivated:
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case ESMAbilityDisplayState::Hidden:
			SetVisibility(ESlateVisibility::Collapsed);
			break;
		default:
			break;
	}
}
