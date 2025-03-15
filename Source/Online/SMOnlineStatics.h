// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMOnlineStatics.generated.h"

UCLASS()
class STEREOMIX_API USMOnlineStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Online")
	static bool IsValidUniqueNetId(const FUniqueNetIdRepl& UniqueNetId)
	{
		return UniqueNetId.IsValid();
	}
	
	UFUNCTION(BlueprintPure, Category = "StereoMix|Online", DisplayName = "Equal (UniqueNetId)", meta=(CompactNodeTitle = "==", Keywords = "== euqal"))
	static bool IsSameUniqueNetId(const FUniqueNetIdRepl& A, const FUniqueNetIdRepl& B)
	{
		return A == B;
	}

	UFUNCTION(BlueprintPure, Category = "StereoMix|Online")
	static FUniqueNetIdRepl GetUniqueNetIdFromPlayer(const ULocalPlayer* Player)
	{
		return Player->GetPreferredUniqueNetId();
	}

	UFUNCTION(BlueprintPure, Category = "StereoMix|Online")
	static FString GetUniqueNetIdAsString(const FUniqueNetIdRepl& UniqueNetId)
	{
		return UniqueNetId.ToString();
	}

	UFUNCTION(BlueprintPure, Category = "StereoMix|Online")
	static FString GetUniqueNetIdAsDebugString(const FUniqueNetIdRepl& UniqueNetId)
	{
		return UniqueNetId.ToDebugString();
	}

	static FString GetProductUserId(const FUniqueNetIdRepl& UniqueNetId)
	{
		if(UniqueNetId.IsValid())
		{
			const FString String_UserID = UniqueNetId->ToString();
			TArray<FString> Substrings;
			String_UserID.ParseIntoArray(Substrings, TEXT("|"));
			// Check if the split was successful
			if (Substrings.Num() == 2)
			{
				return Substrings[1];
			}
			if(Substrings.Num() == 1)
			{
				return Substrings[0];
			}
			return FString();
		}
		return FString();
	}

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Online", meta = (WorldContext = "WorldContextObject"))
	static FString GetLocalPlayerProductUserId(UObject* WorldContextObject, int32 LocalPlayerIndex)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (!World)
		{
			return FString();
		}
		const ULocalPlayer* Player = GEngine->GetLocalPlayerFromControllerId(World, LocalPlayerIndex);
		return GetProductUserId(Player->GetPreferredUniqueNetId());
	}
};
