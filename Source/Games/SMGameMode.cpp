// Copyright Studio Surround. All Rights Reserved.


#include "SMGameMode.h"


#include "CommonSessionSubsystem.h"
#include "GameMapsSettings.h"
#include "SMGameState.h"
#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Core/SMCharacterSpawnManagerComponent.h"
#include "Match/SMMatchDefinition.h"
#include "Match/SMMatchSubsystem.h"
#include "Player/SMPlayerController.h"
#include "Player/SMPlayerState.h"
#include "System/SMGameSession.h"
#include "UI/SMGameHUD.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMGameMode)

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixGameMode, Log, All);

ASMGameMode::ASMGameMode()
{
	GameSessionClass = ASMGameSession::StaticClass();
	GameStateClass = ASMGameState::StaticClass();
	PlayerStateClass = ASMPlayerState::StaticClass();
	HUDClass = ASMGameHUD::StaticClass();
}

FPrimaryAssetId ASMGameMode::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType("GameMode"), GetFName());
}

UClass* ASMGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const USMCharacterSpawnManagerComponent* CharacterSpawnManager = FindComponentByClass<USMCharacterSpawnManagerComponent>())
	{
		return CharacterSpawnManager->FindCharacterClassForPlayer(Cast<ASMPlayerController>(InController));
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

bool ASMGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// 플레이어가 스폰될 때 플레이어 컨트롤러의 StartSpot을 사용하지 않음
	return false;
}

AActor* ASMGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (const USMCharacterSpawnManagerComponent* CharacterSpawnManager = FindComponentByClass<USMCharacterSpawnManagerComponent>())
	{
		return CharacterSpawnManager->ChoosePlayerStart(Player);
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ASMGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	if (APawn* OldPawn = NewPlayer->GetPawn())
	{
		OldPawn->Destroy();
	}
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void ASMGameMode::RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	if (APawn* OldPawn = NewPlayer->GetPawn())
	{
		OldPawn->Destroy();
	}
	Super::RestartPlayerAtTransform(NewPlayer, SpawnTransform);
}

void ASMGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	NET_LOG_CATEGORY(this, LogStereoMixGameMode, Verbose, TEXT("InitGame: %s, %s"), *MapName, *Options);
	Super::InitGame(MapName, Options, ErrorMessage);

	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::InitializeServerMatch);
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckIsReadyToStart);
	}
}

void ASMGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);
	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

ASMGameSession* ASMGameMode::GetStereoMixSession() const
{
	return Cast<ASMGameSession>(GameSession);
}

void ASMGameMode::BeginPlay()
{
	NET_LOG_CATEGORY(this, LogStereoMixGameMode, Verbose, TEXT("BeginPlay"));
	Super::BeginPlay();

	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckIsReadyToStart);
}

FString ASMGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	// 부모 함수에서 StartSpot을 정하기 때문에 먼저 팀과 캐릭터를 초기화해야 함
	if (ASMPlayerState* StereoMixPS = Cast<ASMPlayerState>(NewPlayerController->PlayerState))
	{
		StereoMixPS->InitTeamAndCharacterType(Options);
	}

	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if (ErrorMessage.IsEmpty())
	{
		if (ASMPlayerState* StereoMixPS = Cast<ASMPlayerState>(NewPlayerController->PlayerState))
		{
			ErrorMessage = StereoMixPS->InitOptions(Options);
		}
	}

	return ErrorMessage;
}

void ASMGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASMGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);
}

void ASMGameMode::PostSeamlessTravel()
{
	UE_LOG(LogStereoMixGameMode, Warning, TEXT("Begin PostSeamlessTravel"));
	Super::PostSeamlessTravel();
	UE_LOG(LogStereoMixGameMode, Warning, TEXT("End PostSeamlessTravel"));
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckIsReadyToStart);
}

void ASMGameMode::CheckIsReadyToStart()
{
	ASMGameState* StereoMixGameState = GetGameState<ASMGameState>();
	if (!StereoMixGameState || StereoMixGameState->IsReadyToStart())
	{
		return;
	}

	if (GetWorld()->GetNumPlayerControllers() == 0)
	{
		// UE_LOG(LogStereoMixGameMode, Verbose, TEXT("No PlayerControllers"));
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckIsReadyToStart);
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ASMPlayerController* StereoMixPC = Cast<ASMPlayerController>(*It))
		{
			if (StereoMixPC->IsReadyToPlay())
			{
				// UE_LOG(LogStereoMixGameMode, Verbose, TEXT("PlayerController(%s): Ready"), *StereoMixPC->GetName());
			}
			else
			{
				// UE_LOG(LogStereoMixGameMode, Verbose, TEXT("PlayerController(%s): Not Ready"), *StereoMixPC->GetName());
				GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckIsReadyToStart);
				return;
			}
		}
	}

	StereoMixGameState->SetReadyToPlay();
}

void ASMGameMode::InitializeServerMatch() const
{
	FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance || World->GetNetMode() != NM_DedicatedServer || World->URL.Map != DefaultMap)
	{
		return;
	}

	USMMatchSubsystem* MatchSubsystem = USMMatchSubsystem::Get(this);
	if (!MatchSubsystem)
	{
		UE_LOG(LogStereoMix, Error, TEXT("Match subsystem not found."));
		return;
	}

	const USMMatchDefinition* MatchDefinition = MatchSubsystem->GetMatchDefinition();
	if (!MatchDefinition)
	{
		UE_LOG(LogStereoMix, Error, TEXT("Match definition not found."));
		return;
	}

	if (MatchSubsystem->GetServerType() == ESMMatchServerType::OnlineMatch)
	{
		StartOnlineMatch(MatchDefinition);
	}
	else
	{
		StartCustomMatch(MatchDefinition);
	}
}

void ASMGameMode::StartOnlineMatch(const USMMatchDefinition* Match) const
{
	int32 DesiredPlayers = 0;
	if (!FParse::Value(FCommandLine::Get(), TEXT("DesiredPlayers="), DesiredPlayers))
	{
		UE_LOG(LogStereoMix, Warning, TEXT("Desired players not specified on command line, using default."));
		DesiredPlayers = -1;
	}

	if (DesiredPlayers < 0)
	{
		DesiredPlayers = Match->MaxPlayerCount;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
		UCommonSession_HostSessionRequest* Request = Match->CreateHostingRequestForOnlineMatch(this);
		if (ensure(Request))
		{
			Request->OnlineMode = ECommonSessionOnlineMode::Online;
			Request->ExtraArgs.Add(TEXT("DesiredPlayers"), FString::FromInt(DesiredPlayers));
			SessionSubsystem->HostSession(nullptr, Request);
		}
	}
}

void ASMGameMode::StartCustomMatch(const USMMatchDefinition* Match) const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
		UCommonSession_HostSessionRequest* Request = Match->CreateHostingRequestForCustomMatch(this);
		if (ensure(Request))
		{
			bool bIsLanGame;
			if (FParse::Bool(FCommandLine::Get(), TEXT("LanGame"), bIsLanGame) && bIsLanGame)
			{
				UE_LOG(LogStereoMix, Log, TEXT("LAN game option found in command line. create session as LAN game."));
			}
			Request->OnlineMode = bIsLanGame ? ECommonSessionOnlineMode::LAN : ECommonSessionOnlineMode::Online;
			SessionSubsystem->HostSession(nullptr, Request);
		}
	}
}
