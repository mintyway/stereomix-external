// Copyright Studio Surround. All Rights Reserved.


#include "SMActiveAbilityDisplaySlotWidget.h"

#include "AbilitySystemGlobals.h"
#include "SMActiveAbilityDisplayEntryWidget.h"
#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Components/WidgetSwitcher.h"

void USMActiveAbilityDisplaySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	EntrySlot->ClearChildren();
}

void USMActiveAbilityDisplaySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMActiveAbilityDisplaySlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void USMActiveAbilityDisplaySlotWidget::SetData(const USMActiveAbilityDisplaySlotData* InSlotData)
{
	SlotData = InSlotData;

	APlayerController* OwningPlayer = GetOwningPlayer();
	AbilitySystemComponent = Cast<USMAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningPlayer));
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("Failed to get AbilitySystemComponent from OwningPlayer."));
		return;
	}

	for (const FSMActiveAbilityDisplayEntryData& EntryData : SlotData->Entries)
	{
		USMActiveAbilityDisplayEntryWidget* EntryWidget = CreateWidget<USMActiveAbilityDisplayEntryWidget>(this, EntryClass);
		EntrySlot->AddChild(EntryWidget);
		EntryWidgets.Add(EntryWidget);
		EntryWidget->SetEntryData(this, EntryData);
	}
	bIsInitialized = true;
}

USMAbilitySystemComponent* USMActiveAbilityDisplaySlotWidget::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void USMActiveAbilityDisplaySlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateState();
}

void USMActiveAbilityDisplaySlotWidget::UpdateState()
{
	if (!bIsInitialized || !AbilitySystemComponent)
	{
		return;
	}

	bool bVisible = false;
	for (auto& EntryWidget : EntryWidgets)
	{
		if (!bVisible)
		{
			ESMAbilityDisplayState OldState, NewState;
			if (EntryWidget->TryUpdateState(OldState, NewState))
			{
				if (NewState != ESMAbilityDisplayState::Hidden)
				{
					if (EntrySlot->GetActiveWidget() != EntryWidget)
					{
						EntrySlot->SetActiveWidget(EntryWidget);
					}
				}
			}
			if (EntryWidget->GetCurrentState() != ESMAbilityDisplayState::Hidden)
			{
				bVisible = true;
			}
		}
		else
		{
			EntryWidget->SetState(ESMAbilityDisplayState::Hidden);
		}
	}
}
