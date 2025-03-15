// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMScreenIndicatorComponent.generated.h"

class USMScreenIndicatorWidget;

UCLASS(meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMScreenIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMScreenIndicatorComponent();

	/** 타겟을 향하는 인디케이터를 추가합니다. */
	void AddScreenIndicator(AActor* TargetActor, const TSubclassOf<USMScreenIndicatorWidget>& ScreenIndicatorClass);

	/** 타겟을 향하는 인디케이터를 제거합니다. */
	void RemoveScreenIndicator(AActor* TargetActor);

protected:
	/** 인디케이터를 갖고 있는 타겟이 파괴되면 호출됩니다. */
	UFUNCTION()
	void OnTargetDestroyed(AActor* DestroyedActor);

	UPROPERTY()
	TMap<AActor*, TObjectPtr<USMScreenIndicatorWidget>> ScreenIndicators;
};
