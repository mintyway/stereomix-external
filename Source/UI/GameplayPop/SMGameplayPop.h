// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMGameplayPop.generated.h"

class USMActorPool;
class USMGameplayPopWidget;
class USMGameplayPopDescriptor;
class UWidgetComponent;

UCLASS(DisplayName = "StereoMix Gameplay Pop")
class STEREOMIX_API ASMGameplayPop : public AActor
{
	GENERATED_BODY()

public:
	ASMGameplayPop();

	void ActivateGameplayPop(USMActorPool* InActorPool, UClass* WidgetClass, const USMGameplayPopDescriptor* InDescriptor);

	void ReleaseGameplayPop();

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay Pop")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay Pop")
	TObjectPtr<USMGameplayPopWidget> ActiveWidget;

	UPROPERTY()
	TWeakObjectPtr<USMActorPool> MyPool;
	
	bool bIsActivated = false;
};
