// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMInputBindableInterface.generated.h"

class UEnhancedInputComponent;

// This class does not need to be modified.
UINTERFACE()
class USMInputBindableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMInputBindableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 플레이어 캐릭터의 SetupPlayerInputComponent에서 호출됩니다. 여기서 입력을 바인드하면됩니다.
	virtual void SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent) = 0;
};
