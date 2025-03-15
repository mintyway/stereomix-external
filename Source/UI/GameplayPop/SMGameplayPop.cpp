// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayPop.h"

#include "SMGameplayPopDescriptors.h"
#include "SMGameplayPopLog.h"
#include "SMGameplayPopWidget.h"
#include "Components/WidgetComponent.h"
#include "Utilities/SMPool.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameplayPop)


ASMGameplayPop::ASMGameplayPop()
{
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawAtDesiredSize(true);

	SetRootComponent(WidgetComponent);
}

void ASMGameplayPop::ActivateGameplayPop(USMActorPool* InActorPool, UClass* WidgetClass, const USMGameplayPopDescriptor* InDescriptor)
{
	if (bIsActivated)
	{
		return;
	}

	check(WidgetComponent);
	check(InActorPool);

	MyPool = InActorPool;
	
	if (!WidgetClass)
	{
		UE_LOG(LogStereoMixGameplayPop, Warning, TEXT("Failed to activate GameplayPop. WidgetClass is nullptr."));
		ReleaseGameplayPop();
		return;
	}

	USMGameplayPopWidget* PopWidget;
	if (UUserWidget* OldWidget = WidgetComponent->GetWidget())
	{
		PopWidget = CastChecked<USMGameplayPopWidget>(OldWidget);
	}
	else
	{
		WidgetComponent->SetWidgetClass(WidgetClass);
		WidgetComponent->InitWidget();
		PopWidget = CastChecked<USMGameplayPopWidget>(WidgetComponent->GetWidget());
	}

	check(PopWidget);
	PopWidget->OnFinishPlay.BindUObject(this, &ASMGameplayPop::ReleaseGameplayPop);
	PopWidget->ActivateGameplayPop(InDescriptor);

	SetActorLocation(InDescriptor->Location);
	SetActorHiddenInGame(false);
	bIsActivated = true;
}

void ASMGameplayPop::ReleaseGameplayPop()
{
	if (!bIsActivated)
	{
		return;
	}

	SetActorLocation(FVector::ZeroVector);
	SetActorHiddenInGame(true);

	if (MyPool.IsValid())
	{
		MyPool->ReturnToPool(this);
	}
	bIsActivated = false;
}
