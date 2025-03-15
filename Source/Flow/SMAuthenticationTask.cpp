// Copyright Studio Surround. All Rights Reserved.


#include "SMAuthenticationTask.h"

#include "OnlineSubsystemUtils.h"
#include "StateTreeExecutionContext.h"
#include "StereoMixLog.h"

EStateTreeRunStatus FSMAuthenticationTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(Context.GetWorld());
	if (!OnlineSubsystem)
	{
		UE_LOG(LogStereoMix, Error, TEXT("Failed to get online subsystem"));
	}
	else
	{
		UE_LOG(LogStereoMix, Log, TEXT("OnlineSubsystem Name: %s"), *OnlineSubsystem->GetSubsystemName().ToString());
	}

	IOnlineSubsystem* NativeOnlineSubsystem = IOnlineSubsystem::GetByPlatform();
	if (!NativeOnlineSubsystem)
	{
		UE_LOG(LogStereoMix, Error, TEXT("Failed to get native online subsystem"));
	}
	else
	{
		UE_LOG(LogStereoMix, Log, TEXT("NativeOnlineSubsystem Name: %s"), *NativeOnlineSubsystem->GetSubsystemName().ToString());
	}

	IOnlineIdentity* IdentityInterface = OnlineSubsystem->GetIdentityInterface().Get();
	
	
	// const IOnlineSubsystem* EOS = Online::GetSubsystem(Context.GetWorld(), "EOSPlus");
	// if (!EOS)
	// {
	// 	UE_LOG(LogStereoMix, Error, TEXT("Failed to get EOS subsystem"));
	// 	return EStateTreeRunStatus::Failed;
	// }
	//
	// IOnlineIdentity* IdentityInterface = EOS->GetIdentityInterface().Get();
	// IdentityInterface->OnLoginCompleteDelegates->AddLambda([&InstanceData, IdentityInterface](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
	// {
	// 	if (bWasSuccessful)
	// 	{
	// 		UE_LOG(LogStereoMix, Log, TEXT("Login successful"));
	// 		
	// 		InstanceData.bIsUserInitializeComplete = true;
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogStereoMix, Error, TEXT("Login failed: %s"), *Error);
	// 	}
	// });
	//
	// FOnlineAccountCredentials Credentials;
	// IdentityInterface->Login(0, Credentials);

	return EStateTreeRunStatus::Running;
}

void FSMAuthenticationTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
}

EStateTreeRunStatus FSMAuthenticationTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	// if (InstanceData.bIsUserInitializeComplete)
	// {
	// 	return EStateTreeRunStatus::Succeeded;
	// }
	return EStateTreeRunStatus::Running;
}
