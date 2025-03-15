// Copyright Studio Surround. All Rights Reserved.


#include "SMStatComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(SMStatComponent)


USMStatComponent::USMStatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USMStatComponent::CopyProperties(UPlayerStateComponent* TargetPlayerStateComponent)
{
	Super::CopyProperties(TargetPlayerStateComponent);
}
