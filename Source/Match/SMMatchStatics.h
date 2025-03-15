// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMMatchStatics.generated.h"


class USMMatchDefinition;

UCLASS()
class STEREOMIX_API USMMatchStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Match")
	static TArray<UObject*> GetMatchDefinitions();

	UFUNCTION(BlueprintCallable, Category = "Match")
	static USMMatchDefinition* FindMatchDefinition(FName MatchDefinitionName);

	static USMMatchDefinition* FindMatchDefinition(const FPrimaryAssetId& MatchAssetId);
};
