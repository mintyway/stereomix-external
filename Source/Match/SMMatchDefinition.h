// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMMatchDefinition.generated.h"

class UCommonSession_HostSessionRequest;


UCLASS(BlueprintType, Const)
class STEREOMIX_API USMMatchDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	TSoftObjectPtr<UTexture2D> CoverImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	int32 MaxPlayerCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	float PreRoundTime = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	float GameRoundTime = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
	float ResultTime = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match", meta=(AllowedTypes="GameMode"))
	FPrimaryAssetId CustomMatchLobbyGameModeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match", meta=(AllowedTypes="Map"))
	FPrimaryAssetId CustomMatchLobbyMapId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match", meta=(AllowedTypes="GameMode"))
	FPrimaryAssetId CharacterSelectGameModeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match", meta=(AllowedTypes="Map"))
	FPrimaryAssetId CharacterSelectMapId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match", meta=(AllowedTypes="GameMode"))
	FPrimaryAssetId MatchGameModeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match", meta=(AllowedTypes="Map"))
	TArray<FPrimaryAssetId> RoundMaps;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintCallable)
	int32 GetRoundCount() const;

	UCommonSession_HostSessionRequest* CreateHostingRequestForOnlineMatch(const UObject* WorldContextObject) const;
	UCommonSession_HostSessionRequest* CreateHostingRequestForCustomMatch(const UObject* WorldContextObject) const;

	FString GetLobbyUrl() const;
	FString GetCharacterSelectUrl() const;
	FString GetRoundUrl(int32 Round) const;
};
