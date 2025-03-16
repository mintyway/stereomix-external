// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SMItemBase.h"
#include "Interfaces/SMCatchInteractionInterface.h"

#include "SMCatchableItem.generated.h"

class USMCatchInteractionComponent_CatchableItem_AttributeChanger;

UCLASS(Abstract)
class STEREOMIX_API ASMCatchableItem : public ASMItemBase, public ISMCatchInteractionInterface
{
	GENERATED_BODY()

public:
	ASMCatchableItem();

	virtual void ActivateItem(AActor* InActivator) override;

	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override PURE_VIRTUAL(ASMCatchableItem::GetCatchInteractionComponent, return nullptr;)

	/** 게임플레이 큐를 재생할때 사용할 태그입니다.*/
	UPROPERTY(EditAnywhere, Category = "Design", meta = (Categories = "GameplayCue"))
	FGameplayTag SmashGCTag;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
