// Copyright Studio Surround. All Rights Reserved.


#include "SMFlowComponent.h"

#include "ControlFlowManager.h"
#include "StereoMixLog.h"


void USMFlowComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(!FlowId.IsEmpty()))
	{
		FControlFlow& NewFlow = FControlFlowStatics::Create(this, FlowId);
		MakeFlow(NewFlow);
		NewFlow.OnNodeComplete().AddUObject(this, &ThisClass::OnNodeComplete);
		NewFlow.OnFlowComplete().AddUObject(this, &ThisClass::OnFlowComplete);
		
		ActiveFlow = NewFlow.AsShared();
	}
}

void USMFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool USMFlowComponent::ShouldShowLoadingScreen(FString& OutReason)
{
	if (bShouldShowLoadingScreen)
	{
		OutReason = DefaultLoadingReason;
		if (ActiveFlow.IsValid())
		{
			const TOptional<FString> StepDebugName = ActiveFlow->GetCurrentStepDebugName();
			if (StepDebugName.IsSet())
			{
				OutReason = StepDebugName.GetValue();
			}
		}
		return true;
	}
	return false;
}

void USMFlowComponent::ExecuteFlow() const
{
	if (ActiveFlow.IsValid())
	{
		ActiveFlow->ExecuteFlow();
		UE_LOG(LogStereoMixFlow, Warning, TEXT("[%s] Flow executed"), *FlowId);
	}
}

void USMFlowComponent::OnNodeComplete()
{
	UE_LOG(LogStereoMixFlow, Verbose, TEXT("[%s] OnNodeComplete"), *FlowId);
}

void USMFlowComponent::OnFlowComplete()
{
	UE_LOG(LogStereoMixFlow, Verbose, TEXT("[%s] OnFlowComplete"), *FlowId);
}
