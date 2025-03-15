// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"

#include "SMGameMode.generated.h"

class USMMatchDefinition;
class ASMGameSession;
class ASMGameState;
enum class ECommonSessionOnlineMode : uint8;

/**
 * Post login event, triggered when a player or bot joins the game as well as after seamless and non seamless travel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FSMOnGameModePlayerInitialized, AGameModeBase* GameMode, AController* NewPlayer);

/**
 * StereoMix Game Mode
 */
UCLASS(Config = Game, DisplayName = "StereoMix Game Mode", Category = "StereoMix|GameMode")
class STEREOMIX_API ASMGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	ASMGameMode();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	virtual void RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;

	ASMGameSession* GetStereoMixSession() const;

	// Delegate called on player initialization, described above
	FSMOnGameModePlayerInitialized OnGameModePlayerInitialized;

protected:
	virtual void BeginPlay() override;
	/** 플레이어 입장시 전송된 닉네임으로 플레이어 스테이트를 초기화해줍니다. */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;

	virtual void PostSeamlessTravel() override;

private:
	/** 이 함수가 호출되면 매 틱 마다 플레이어의 로드 여부를 체크합니다. 이후 모든 플레이어가 로드되면 델리게이트가 브로드캐스트됨과 동시에 종료됩니다. */
	void CheckIsReadyToStart();
	
	void InitializeServerMatch() const;

	/** 매치메이킹 매치를 시작합니다. */
	void StartOnlineMatch(const USMMatchDefinition* Match) const;

	/** 커스텀 매치를 시작합니다. */
	void StartCustomMatch(const USMMatchDefinition* Match) const;
};
