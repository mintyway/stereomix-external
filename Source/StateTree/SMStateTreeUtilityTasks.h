// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "SMStateTreeUtilityTasks.generated.h"

UENUM(BlueprintType)
enum class ESMStateTreeLogVerbosity : uint8
{
	Verbose,
	Log,
	Warning,
	Error
};

UENUM(BlueprintType)
enum class ESMStateTreeLogTiming : uint8
{
	Enter,
	Exit,
};

USTRUCT()
struct STEREOMIX_API FSMStateTreePrintLogTaskData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> OwnerActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FString Message;
};

USTRUCT(Blueprintable, DisplayName = "Print Log")
struct STEREOMIX_API FSMStateTreePrintLogTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSMStateTreePrintLogTaskData;
	
	FSMStateTreePrintLogTask()
	{
		bShouldStateChangeOnReselect = false;
	}

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	void PrintLog(FStateTreeExecutionContext& Context) const;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	ESMStateTreeLogTiming Timing = ESMStateTreeLogTiming::Enter;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	ESMStateTreeLogVerbosity Verbosity = ESMStateTreeLogVerbosity::Log;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bPrintToScreen = false;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bPrintToLog = true;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bUseNetLog = false;
};
