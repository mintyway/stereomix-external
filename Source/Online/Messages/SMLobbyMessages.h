// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMLobbyMessages.generated.h"

USTRUCT(BlueprintType, DisplayName="StereoMix Lobby Event Message")
struct STEREOMIX_API FSMLobbyEventMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Lobbies")
	FUniqueNetIdRepl LobbyId;
};

USTRUCT(DisplayName="StereoMix Lobby Member Event Message")
struct STEREOMIX_API FSMLobbyMemberEventMessage : public FSMLobbyEventMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Lobbies")
	FUniqueNetIdRepl MemberId;

	UPROPERTY(BlueprintReadWrite, Category="Lobbies")
	FString DisplayUserName;
};

USTRUCT(DisplayName="StereoMix Lobby Chat Message")
struct STEREOMIX_API FSMLobbyChatMessage : public FSMLobbyEventMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Lobbies|Chat")
	uint8 bIsOwner : 1 = false;
	
	UPROPERTY(BlueprintReadWrite, Category="Lobbies|Chat")
	int32 Team = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Lobbies|Chat")
	FString Message;
};
