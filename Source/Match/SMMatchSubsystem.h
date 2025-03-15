// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMMatchSubsystem.generated.h"

class USMMatchDefinition;

UENUM(BlueprintType)
enum class ESMMatchServerType : uint8
{
	None,
	OnlineMatch,
	CustomMatch,
};

UENUM()
enum class ESMMatchState : int32
{
	Lobby,
	InProgress
};

namespace StereoMix::Tags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MatchPhase);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MatchPhase_Lobby);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MatchPhase_CharacterSelect);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MatchPhase_Round);
}

/**
 * 서버의 전반적인 게임 흐름을 관리하는 서브시스템입니다. 서버에서만 존재합니다.
 */
UCLASS(DisplayName = "StereoMix Match Subsystem", Category = "StereoMix|Match")
class STEREOMIX_API USMMatchSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USMMatchSubsystem();

	static USMMatchSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	ESMMatchServerType GetServerType() const { return ServerType; }
	ESMMatchState GetMatchState() const { return MatchState; }
	const FPrimaryAssetId& GetMatchId() const { return MatchId; }
	const USMMatchDefinition* GetMatchDefinition() const { return MatchDefinition.Get(); }

	ESMMatchState UpdateMatchState();
	
protected:
	void InitMatch();

	void UpdateMatchSession() const;
	
	void HandlePostLoadMap(UWorld* World) const;

private:
	ESMMatchServerType ServerType;
	ESMMatchState MatchState;
	FPrimaryAssetId MatchId;

	UPROPERTY()
	TObjectPtr<USMMatchDefinition> MatchDefinition;
};
