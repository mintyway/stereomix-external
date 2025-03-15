// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMChatComponent.h"
#include "Player/SMPlayerState.h"

#include "SMChatMessage.generated.h"

USTRUCT(BlueprintType)
struct FSMChatMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category=Chat)
	FSMChatFromInfo FromInfo;

	UPROPERTY(BlueprintReadWrite, Category=Chat)
	bool bIsTeamMessage = false;

	UPROPERTY(BlueprintReadWrite, Category=Chat)
	FString Message;

	// 디버깅을 위한 문자열을 반환합니다.
	STEREOMIX_API FString GetDebugString() const;
};
