// Copyright Studio Surround. All Rights Reserved.

#include "SMChatMessageHelpers.h"

#include "SMChatMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMChatMessageHelpers)

FString FSMChatMessage::GetDebugString() const
{
	FString Str = FString::Printf(TEXT("From: %s (%s), Message: %s"), *FromInfo.DisplayName,
		bIsTeamMessage ? TEXT("TEAM") : TEXT("ALL"), *Message);
	return Str;
}
