// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMGlobalValidation.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMGlobalValidation : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Validation")
	static bool ValidateName(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Validation")
	static bool ValidateRoomCode(const FString& RoomCode);
};
