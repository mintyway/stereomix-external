// Copyright Studio Surround. All Rights Reserved.


#include "SMChatComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "SMChatMessage.h"
#include "Player/SMPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixChat, Log, All);

UE_DEFINE_GAMEPLAY_TAG_STATIC(GameplayMessage_Chat, "GameplayMessage.Chat");

USMChatComponent::USMChatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USMChatComponent::ServerSendMessage_Implementation(bool bIsTeamMessage, const FString& Message)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		const ASMPlayerController* OwnerController = CastChecked<ASMPlayerController>(GetOwner());
		const ESMTeam OwnerTeam = OwnerController->GetTeam();
		if (bIsTeamMessage && OwnerTeam == ESMTeam::None)
		{
			UE_LOG(LogStereoMixChat, Warning, TEXT("Player is not in a team, cannot send team message."));
			return;
		}

		FSMChatFromInfo FromInfo;
		const ASMPlayerState* OwnerPlayerState = OwnerController->GetPlayerState<ASMPlayerState>();
		FromInfo.UserId = OwnerPlayerState->GetUniqueId();
		// FromInfo.ProfileId = 
		FromInfo.DisplayName = OwnerPlayerState->GetPlayerName();

		UE_LOG(LogStereoMixChat, Log, TEXT("[%s] %s: %s"), bIsTeamMessage ? TEXT("TEAM") : TEXT("ALL"), *FromInfo.DisplayName, *Message);

		for (auto It = GetWorld()->GetControllerIterator(); It; ++It) 
		{
			ASMPlayerController* TargetController = Cast<ASMPlayerController>(It->Get());
			if (!TargetController)
			{
				continue;
			}

			USMChatComponent* ChatListener = TargetController->GetComponentByClass<USMChatComponent>();
			if (!ChatListener)
			{
				return;
			}

			if (bIsTeamMessage)
			{
				const ESMTeam ListenerTeam = TargetController->GetTeam();
				if (OwnerTeam == ListenerTeam)
				{
					ChatListener->ClientReceiveMessage(FromInfo, bIsTeamMessage, Message);
				}
			}
			else
			{
				ChatListener->ClientReceiveMessage(FromInfo, bIsTeamMessage, Message);
			}
		}
	}
#endif
}

void USMChatComponent::ClientReceiveMessage_Implementation(FSMChatFromInfo FromInfo, bool bIsTeamMessage, const FString& Message)
{
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);

	FSMChatMessage ChatMessage;
	ChatMessage.FromInfo = FromInfo;
	ChatMessage.bIsTeamMessage = bIsTeamMessage;
	ChatMessage.Message = Message;
	GameplayMessageSubsystem.BroadcastMessage(GameplayMessage_Chat, ChatMessage);
}
