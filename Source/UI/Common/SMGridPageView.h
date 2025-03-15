// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "Components/UniformGridPanel.h"
#include "SMGridPageView.generated.h"

class USMPageEntryObject;
class USMGridPage;
class IUserObjectListEntry;
class UCommonActivatableWidgetSwitcher;

/**
 * Stereo Mix Common Page View
 */
UCLASS(Abstract, Blueprintable, ClassGroup = UI, meta = (Category = "Common UI", DisableNativeTick))
class STEREOMIX_API USMGridPageView : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	USMGridPageView();

	UFUNCTION(BlueprintPure, Category = Page)
	int GetPageColumn() const { return PageColumn; }

	UFUNCTION(BlueprintPure, Category = Page)
	int GetPageRow() const { return PageRow; }

	UFUNCTION(BlueprintPure, Category = Page)
	int GetMaxEntriesPerPage() const { return PageRow * PageColumn; }

	UFUNCTION(BlueprintPure, Category = Page)
	int GetActivePageIndex() const { return ActivePageIndex; }

	UFUNCTION(BlueprintPure, Category = Page)
	int GetPageCount() const { return PageCount; }

	UFUNCTION(BlueprintCallable, Category = Page)
	bool NextPage();

	UFUNCTION(BlueprintCallable, Category = Page)
	bool PreviousPage();

	UFUNCTION(BlueprintCallable, Category = Page)
	void Refresh(int PageIndex = 0);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Page)
	TArray<TObjectPtr<USMPageEntryObject>> PageItems;

protected:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Page, meta = (DisplayName = "On Refresh Page"))
	void BP_OnRefreshPage();
	virtual void NativeOnRefreshPage();

private:
	UPROPERTY(BlueprintReadOnly, Category = Page, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UUniformGridPanel> GridView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess, MustImplement = "/Script/StereoMix.SMPageEntry"))
	TSubclassOf<UUserWidget> EntryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess))
	TSubclassOf<UUserWidget> EmptyEntryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess, ClampMin = 1))
	int PageColumn = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess, ClampMin = 1))
	int PageRow = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess))
	TSubclassOf<USMPageEntryObject> PreviewEntryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess, ClampMin = 0, ClampMax = 100))
	int PreviewEntryCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Page, meta = (DesignerRebuild, AllowPrivateAccess, ClampMin = 0))
	int PreviewPageIndex = 0;
	
	UPROPERTY(Transient)
	int PageCount = 0;

	UPROPERTY(Transient)
	int ActivePageIndex = 0;

	UPROPERTY(Transient)
	FUserWidgetPool EntryWidgetPool;
};
