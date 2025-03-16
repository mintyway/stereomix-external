// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMLocalTeam.h"
#include "Data/SMTeam.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SMTeamBlueprintLibrary.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMTeamBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 팀 정보를 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "Team")
	static ESMTeam GetTeam(const AActor* SourceActor);

	/** 같은 팀 여부를 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "Team")
	static bool IsSameTeam(const AActor* lhs, const AActor* rhs);

	/** 액터의 로컬 팀 정보를 반환합니다. 대상이 유효하지 않은 경우에는 다른 팀으로 반환합니다. 클라이언트에서 실행되어야합니다. */
	UFUNCTION(BlueprintPure, Category = "Team")
	static ESMLocalTeam GetLocalTeam(AActor* TargetActor);

	/** 로컬에서 플레이하고 있는 팀과 시전자의 팀이 같은지 확인합니다. 클라이언트에서 실행되어야합니다. */
	UFUNCTION(BlueprintPure, Category = "Team")
	static bool IsSameLocalTeam(AActor* TargetActor);
};
