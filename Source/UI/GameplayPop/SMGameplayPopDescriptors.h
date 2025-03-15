// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/SMTeam.h"
#include "SMGameplayPopDescriptors.generated.h"

UCLASS(BlueprintType, DisplayName = "Gameplay Pop Descriptor")
class STEREOMIX_API USMGameplayPopDescriptor : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	AActor* Instigator;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	FVector Location;
};


UCLASS(BlueprintType, DisplayName = "Gameplay Damage Pop Descriptor")
class STEREOMIX_API USMGameplayDamagePopDescriptor : public USMGameplayPopDescriptor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	int32 Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	bool bCritical;
};


UCLASS(BlueprintType, DisplayName = "Gameplay Item Pop Descriptor")
class STEREOMIX_API USMGameplayItemPopDescriptor : public USMGameplayPopDescriptor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	FText DisplayName;
};


UCLASS(BlueprintType, DisplayName = "Gameplay Buff Pop Descriptor")
class STEREOMIX_API USMGameplayBuffPopDescriptor : public USMGameplayPopDescriptor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	bool bIsDebuff;
};


UCLASS(BlueprintType, DisplayName = "Gameplay Tile Pop Descriptor")
class STEREOMIX_API USMGameplayTilePopDescriptor : public USMGameplayPopDescriptor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Pop")
	int32 Count;
};
