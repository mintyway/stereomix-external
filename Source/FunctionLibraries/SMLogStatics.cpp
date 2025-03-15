// Copyright Studio Surround. All Rights Reserved.


#include "SMLogStatics.h"

#include "Blueprint/UserWidget.h"
#include "Utilities/SMLog.h"

void USMLogStatics::PrintNetInfoString(AActor* Actor, const FString& LogMessage, const bool bPrintToScreen, const bool bPrintToLog)
{
	if (bPrintToScreen)
	{
		NET_VLOG(Actor, 0, 4.0f, TEXT("%s"), *LogMessage);
	}
	if (bPrintToLog)
	{
		NET_LOG(Actor, Log, TEXT("%s"), *LogMessage);
	}
}

void USMLogStatics::PrintNetInfoText(AActor* Actor, const FText LogMessage, const bool bPrintToScreen, const bool bPrintToLog)
{
	PrintNetInfoString(Actor, LogMessage.ToString(), bPrintToScreen, bPrintToLog);
}

void USMLogStatics::PrintNetWarningString(AActor* Actor, const FString& LogMessage, const bool bPrintToScreen, const bool bPrintToLog)
{
	if (bPrintToScreen)
	{
		NET_VLOG(Actor, 0, 4.0f, TEXT("%s"), *LogMessage);
	}
	if (bPrintToLog)
	{
		NET_LOG(Actor, Warning, TEXT("%s"), *LogMessage);
	}
}

void USMLogStatics::PrintNetWarningText(AActor* Actor, const FText LogMessage, const bool bPrintToScreen, const bool bPrintToLog)
{
	PrintNetWarningString(Actor, LogMessage.ToString(), bPrintToScreen, bPrintToLog);
}

void USMLogStatics::PrintNetErrorString(AActor* Actor, const FString& LogMessage, const bool bPrintToScreen, const bool bPrintToLog)
{
	if (bPrintToScreen)
	{
		NET_VLOG(Actor, 0, 4.0f, TEXT("%s"), *LogMessage);
	}
	if (bPrintToLog)
	{
		NET_LOG(Actor, Error, TEXT("%s"), *LogMessage);
	}
}

void USMLogStatics::PrintNetErrorText(AActor* Actor, const FText LogMessage, const bool bPrintToScreen, const bool bPrintToLog)
{
	PrintNetErrorString(Actor, LogMessage.ToString(), bPrintToScreen, bPrintToLog);
}

AActor* USMLogStatics::GetContextActor(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	if (AActor* ContextActor = Cast<AActor>(WorldContextObject))
	{
		return ContextActor;
	}
	if (UActorComponent* ContextComponent = Cast<UActorComponent>(WorldContextObject))
	{
		return ContextComponent->GetOwner();
	}
	if (UUserWidget* ContextWidget = Cast<UUserWidget>(WorldContextObject))
	{
		return ContextWidget->GetOwningPlayer();
	}

	UE_LOG(LogStereoMixNetwork, Warning, TEXT("Could not find an actor to use as the context for logging."));
	return nullptr;
}
