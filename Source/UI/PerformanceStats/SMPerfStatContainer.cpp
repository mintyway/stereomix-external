// Copyright Studio Surround. All Rights Reserved.


#include "SMPerfStatContainer.h"

#include "SMPerfStatWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Settings/SMSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPerfStatContainer)

void USMPerfStatContainer::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();
	USMSettingsLocal::Get()->OnPerfStatSettingsChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void USMPerfStatContainer::NativeDestruct()
{
	USMSettingsLocal::Get()->OnPerfStatSettingsChanged().RemoveAll(this);
	Super::NativeDestruct();
}

void USMPerfStatContainer::UpdateVisibilityOfChildren()
{
	USMSettingsLocal* UserSettings = USMSettingsLocal::Get();

	const bool bShowTextWidgets = StatDisplayModeFilter == ESMStatDisplayMode::TextOnly || StatDisplayModeFilter == ESMStatDisplayMode::TextAndGraph;
	const bool bShowGraphWidgets = StatDisplayModeFilter == ESMStatDisplayMode::GraphOnly || StatDisplayModeFilter == ESMStatDisplayMode::TextAndGraph;

	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget) {
		if (USMPerfStatWidget* TypedWidget = Cast<USMPerfStatWidget>(Widget))
		{
			const ESMStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
				case ESMStatDisplayMode::Hidden:
					bShowWidget = false;
					break;
				case ESMStatDisplayMode::TextOnly:
					bShowWidget = bShowTextWidgets;
					break;
				case ESMStatDisplayMode::GraphOnly:
					bShowWidget = bShowGraphWidgets;
					break;
				case ESMStatDisplayMode::TextAndGraph:
					bShowWidget = bShowTextWidgets || bShowGraphWidgets;
					break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}
