// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvents.h"
#include "UObject/Object.h"
#include "FlowEvent.generated.h"

USTRUCT(BlueprintType, DisplayName = "Flow Event")
struct FSMFlowEvent : public FStateTreeEvent
{
	GENERATED_BODY()

	/** 태그 컨테이너에 저장할 지 여부를 나타냅니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 bSaveToContainer : 1 = false;
};
