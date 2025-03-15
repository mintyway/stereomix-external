// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUILibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMCommonUILibrary.generated.h"

struct FUIActionTag;
struct FUIActionBindingHandle;
class UInputAction;
class UCommonUserWidget;

DECLARE_DYNAMIC_DELEGATE_OneParam(FInputActionExecutedDelegate, FName, ActionName);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInputActionTagExecutedDelegate, const FUIActionTag&, ActionTag);

/**
 * StereoMix Common UI Library 
 */
UCLASS()
class STEREOMIX_API USMCommonUILibrary : public UCommonUILibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "StereoMix Common UI Library", meta=(WorldContext = "InWidget"))
	static void RegisterBinding(UCommonUserWidget* InWidget, const UInputAction* InputAction, bool bShouldDisplayInActionBar, bool bShouldConsumeInput, const FInputActionExecutedDelegate& Callback, FUIActionBindingHandle& BindingHandle);

	UFUNCTION(BlueprintCallable, Category = "StereoMix Common UI Library", meta=(WorldContext = "InWidget"))
	static void RegisterBindingWithActionTag(UCommonUserWidget* InWidget, const FUIActionTag& ActionTag, bool bShouldDisplayInActionBar, bool bShouldConsumeInput, const FInputActionTagExecutedDelegate& Callback, FUIActionBindingHandle& BindingHandle);

	UFUNCTION(BlueprintCallable, Category = "StereoMix Common UI Library", meta=(WorldContext = "InWidget"))
	static void AddBinding(UCommonUserWidget* InWidget, FUIActionBindingHandle BindingHandle);
	
	UFUNCTION(BlueprintCallable, Category = "StereoMix Common UI Library", meta=(WorldContext = "InWidget"))
	static void RemoveBinding(UCommonUserWidget* InWidget, FUIActionBindingHandle BindingHandle);
};
