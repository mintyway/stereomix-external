// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "SMVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FSMVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;

UCLASS()
class STEREOMIX_API USMVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Messages")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Messages")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Messages")
	static bool IsFromLocalPlayer(const FSMVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Messages")
	static FGameplayCueParameters VerbMessageToCueParameters(const FSMVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Messages")
	static FSMVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
