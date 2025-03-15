// Copyright Studio Surround. All Rights Reserved.


#include "SMStateTreeUtilityTasks.h"

#include "Logging/StructuredLog.h"
#include "StateTreeExecutionContext.h"
#include "StereoMixLog.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMStateTreeUtilityTasks)

EStateTreeRunStatus FSMStateTreePrintLogTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Timing == ESMStateTreeLogTiming::Enter)
	{
		PrintLog(Context);
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FSMStateTreePrintLogTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Timing == ESMStateTreeLogTiming::Exit)
	{
		PrintLog(Context);
	}
}

void FSMStateTreePrintLogTask::PrintLog(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	AActor* OwnerActor = InstanceData.OwnerActor;
	FString OwnerName = OwnerActor ? OwnerActor->GetName() : TEXT("None");
	FString Message = InstanceData.Message;

	if (bUseNetLog)
	{
		switch (Verbosity)
		{
			case ESMStateTreeLogVerbosity::Verbose:
				NET_LOG(OwnerActor, Verbose, TEXT("%s >> %s"), *OwnerName, *Message);
				break;
			case ESMStateTreeLogVerbosity::Log:
				NET_LOG(OwnerActor, Log, TEXT("%s >> %s"), *OwnerName, *Message);
				break;
			case ESMStateTreeLogVerbosity::Warning:
				NET_LOG(OwnerActor, Warning, TEXT("%s >> %s"), *OwnerName, *Message);
				break;
			case ESMStateTreeLogVerbosity::Error:
				NET_LOG(OwnerActor, Error, TEXT("%s >> %s"), *OwnerName, *Message);
				break;
		}
	}
	else
	{
		switch (Verbosity)
		{
			case ESMStateTreeLogVerbosity::Verbose:
				UE_LOGFMT(LogStereoMixFlow, Verbose, "[{Owner}] >> {Message}", *OwnerName, *Message);
				break;
			case ESMStateTreeLogVerbosity::Log:
				UE_LOGFMT(LogStereoMixFlow, Log, "[{Owner}] >> {Message}", *OwnerName, *Message);
				break;
			case ESMStateTreeLogVerbosity::Warning:
				UE_LOGFMT(LogStereoMixFlow, Warning, "[{Owner}] >> {Message}", *OwnerName, *Message);
				break;
			case ESMStateTreeLogVerbosity::Error:
				UE_LOGFMT(LogStereoMixFlow, Error, "[{Owner}] >> {Message}", *OwnerName, *Message);
				break;
		}
	}

	// print to screen
	if (bPrintToScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.f, FColor::White, Message);
	}
}
