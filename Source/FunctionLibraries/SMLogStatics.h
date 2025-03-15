// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMLogStatics.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMLogStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Log", meta = (DefaultToSelf = "Actor", Keywords = "log", AdvancedDisplay = "2", DevelopmentOnly))
	static void PrintNetInfoString(AActor* Actor, const FString& LogMessage, const bool bPrintToScreen = true, const bool bPrintToLog = true);
	
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Log", meta = (DefaultToSelf = "Actor", Keywords = "log", AdvancedDisplay = "2", DevelopmentOnly))
	static void PrintNetInfoText(AActor* Actor, FText LogMessage, const bool bPrintToScreen = true, const bool bPrintToLog = true);
	
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Log", meta = (DefaultToSelf = "Actor", Keywords = "log", AdvancedDisplay = "2", DevelopmentOnly))
	static void PrintNetWarningString(AActor* Actor, const FString& LogMessage, const bool bPrintToScreen = true, const bool bPrintToLog = true);

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Log", meta = (DefaultToSelf = "Actor", Keywords = "log", AdvancedDisplay = "2", DevelopmentOnly))
	static void PrintNetWarningText(AActor* Actor, FText LogMessage, const bool bPrintToScreen = true, const bool bPrintToLog = true);

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Log", meta = (DefaultToSelf = "Actor", Keywords = "log", AdvancedDisplay = "2", DevelopmentOnly))
	static void PrintNetErrorString(AActor* Actor, const FString& LogMessage, const bool bPrintToScreen = true, const bool bPrintToLog = true);

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Log", meta = (DefaultToSelf = "Actor", Keywords = "log", AdvancedDisplay = "2", DevelopmentOnly))
	static void PrintNetErrorText(AActor* Actor, FText LogMessage, const bool bPrintToScreen = true, const bool bPrintToLog = true);

	static AActor* GetContextActor(UObject* WorldContextObject);
};
