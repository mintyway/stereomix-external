// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data//SMTeam.h"

#include "SMTeamComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSMOnTeamChanged, AActor* TeamActor, ESMTeam PreviousTeam, ESMTeam NewTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSMOnTeamChangedDynamic, AActor*, TeamActor, ESMTeam, PreviousTeam, ESMTeam, NewTeam);


UCLASS(meta = (BlueprintSpawnableComponent))
class STEREOMIX_API USMTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTeamComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Team")
	ESMTeam GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetTeam(ESMTeam InTeam);

	UPROPERTY(BlueprintAssignable)
	FSMOnTeamChangedDynamic K2_OnTeamChangedDynamic;
	FSMOnTeamChanged OnTeamChanged;

protected:
	UFUNCTION()
	void OnRep_Team(ESMTeam PreviousTeam);

private:
	/** 자신이 속한 팀을 나타냅니다.*/
	UPROPERTY(EditAnywhere, Category = "Team", ReplicatedUsing = "OnRep_Team")
	ESMTeam Team;

	friend class ASMPlayerState;
};
