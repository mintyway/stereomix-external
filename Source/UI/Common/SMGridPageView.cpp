// Copyright Studio Surround. All Rights Reserved.


#include "SMGridPageView.h"

#include "SMPageEntryInterface.h"
#include "SMPagePreviewEntryObject.h"

USMGridPageView::USMGridPageView()
	: EntryWidgetPool(*this)
{
}

bool USMGridPageView::NextPage()
{
	Refresh(ActivePageIndex + 1);
	return true;
}

bool USMGridPageView::PreviousPage()
{
	Refresh(ActivePageIndex - 1);
	return true;
}

void USMGridPageView::Refresh(const int PageIndex)
{
	for (UWidget* Child : GridView->GetAllChildren())
	{
		UUserWidget* ChildWidget = Cast<UUserWidget>(Child);
		EntryWidgetPool.Release(ChildWidget, true);
	}
	GridView->ClearChildren();

	PageCount = FMath::CeilToInt32(static_cast<float>(PageItems.Num()) / (PageRow * PageColumn));
	ActivePageIndex = FMath::Clamp(PageIndex, 0, PageCount - 1);

	const int StartIndexInclude = ActivePageIndex * PageRow * PageColumn;
	const int EndIndexExclude = FMath::Min(StartIndexInclude + PageRow * PageColumn, PageItems.Num());

	for (int i = 0; i < GetMaxEntriesPerPage(); ++i)
	{
		UUserWidget* EntryWidget = nullptr;
		if (const int ItemIndex = StartIndexInclude + i; ItemIndex < EndIndexExclude)
		{
			if (EntryWidgetClass && EntryWidgetClass->ImplementsInterface(USMPageEntryInterface::StaticClass()))
			{
				EntryWidget = EntryWidgetPool.GetOrCreateInstance(EntryWidgetClass);
				ISMPageEntryInterface::SetItemObject(EntryWidget, PageItems[ItemIndex]);
			}
		}
		else
		{
			if (EmptyEntryWidgetClass)
			{
				EntryWidget = EntryWidgetPool.GetOrCreateInstance(EmptyEntryWidgetClass);
			}
		}

		if (EntryWidget)
		{
			GridView->AddChildToUniformGrid(EntryWidget, i / PageColumn, i % PageColumn);
		}
	}

	NativeOnRefreshPage();
}

void USMGridPageView::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	EntryWidgetPool.ReleaseAllSlateResources();
}

void USMGridPageView::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (GridView)
	{
		GridView->ClearChildren();
#if WITH_EDITOR
		if (IsDesignTime())
		{
			PageItems.Reset();
			if (PreviewEntryItemClass && PreviewEntryItemClass->IsChildOf<USMPageEntryObject>())
			{
				for (int i = 0; i < PreviewEntryCount; ++i)
				{
					PageItems.Add(NewObject<USMPageEntryObject>(this, PreviewEntryItemClass));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PreviewEntryItemClass is not valid."));
				for (int i = 0; i < PreviewEntryCount; ++i)
				{
					PageItems.Add(NewObject<USMPagePreviewEntryObject>(this));
				}
			}
			Refresh(PreviewPageIndex);
		}
#endif
	}
}

void USMGridPageView::NativeConstruct()
{
	Super::NativeConstruct();
	Refresh();
}

void USMGridPageView::NativeOnRefreshPage()
{
	BP_OnRefreshPage();
}
