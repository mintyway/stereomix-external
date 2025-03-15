// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "Components/PlayerStateComponent.h"
#include "SMChatComponent.generated.h"


class ASMPlayerState;

USTRUCT(BlueprintType)
struct FSMChatFromInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FUniqueNetIdRepl UserId;

	UPROPERTY(BlueprintReadOnly)
	FName ProfileId;

	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;
};

UCLASS(DisplayName = "StereoMix Chat Component")
class STEREOMIX_API USMChatComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	USMChatComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(Server, Reliable)
	void ServerSendMessage(bool bIsTeamMessage, const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveMessage(FSMChatFromInfo FromInfo, bool bIsTeamMessage, const FString& Message);
};
