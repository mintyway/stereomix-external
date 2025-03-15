// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "SMCharacterSelectLog.h"
#include "SMCharacterSelectPlayerState.h"
#include "SMCharacterSelectState.h"
#include "StereoMixLog.h"
#include "Components/Core/SMCountdownTimerComponent.h"
#include "Match/SMMatchDefinition.h"
#include "Match/SMMatchSubsystem.h"
#include "Player/SMCharacterSelectPlayerController.h"
#include "System/SMAssetManager.h"
#include "System/SMGameInstance.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectMode::ASMCharacterSelectMode()
{
	bUseSeamlessTravel = true;
	GameStateClass = ASMCharacterSelectState::StaticClass();
	PlayerStateClass = ASMCharacterSelectPlayerState::StaticClass();
	PlayerControllerClass = ASMCharacterSelectPlayerController::StaticClass();
}

void ASMCharacterSelectMode::InitGameState()
{
	Super::InitGameState();
	CharacterSelectState = Cast<ASMCharacterSelectState>(GameState);

	if (USMGameInstance* GameInstance = GetGameInstance<USMGameInstance>(); HasAuthority() && GameInstance)
	{
		GameInstance->SyncCharacterStatsWithServer();
	}
}

void ASMCharacterSelectMode::StartPlay()
{
	Super::StartPlay();

	RemainingWaitingTime = 30;
	GetWorldTimerManager().SetTimer(WaitingTimerHandle, this, &ASMCharacterSelectMode::WaitingTimerTick, 1.0f, true, 3.0f);
}

bool ASMCharacterSelectMode::IsAllPlayerReadyToSelect() const
{
	if (CharacterSelectState.IsValid())
	{
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			const ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
			if (CharacterSelectPlayerState && CharacterSelectPlayerState->GetCurrentState() == ECharacterSelectPlayerStateType::Loading)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void ASMCharacterSelectMode::ImmediateStart()
{
	if (!CharacterSelectState.IsValid() || CharacterSelectState->GetCurrentState() != ECharacterSelectionStateType::Select)
	{
		return;
	}

	CharacterSelectState->GetCountdownTimer()->CancelCountdown();
	OnCharacterSelectCountdownFinished();
}

void ASMCharacterSelectMode::WaitingTimerTick()
{
	RemainingWaitingTime--;

	if (IsAllPlayerReadyToSelect() || RemainingWaitingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(WaitingTimerHandle);
		NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Verbose, TEXT("Character select countdown started."))
		CharacterSelectState->SetCurrentState(ECharacterSelectionStateType::Select);

		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddUniqueDynamic(this, &ASMCharacterSelectMode::OnCharacterSelectCountdownFinished);
		CharacterSelectState->GetCountdownTimer()->StartCountdown(CharacterSelectState->CharacterSelectCountdownTime);
		return;
	}

	NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Verbose, TEXT("Waiting for players to join. %d seconds remaining."), RemainingWaitingTime)
}

void ASMCharacterSelectMode::OnCharacterSelectCountdownFinished()
{
	// CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.RemoveDynamic(this, &ASMCharacterSelectMode::OnCountdownFinished);
	CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddUniqueDynamic(this, &ASMCharacterSelectMode::StartGame);

	NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Verbose, TEXT("Character select countdown finished."))

	CharacterSelectState->SetCurrentState(ECharacterSelectionStateType::End);
	CharacterSelectState->GetCountdownTimer()->StartCountdown(5);

	if (CharacterSelectState.IsValid())
	{
		auto InitializeCharacterTypeMap = [] {
			TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>> CharacterTypesMap;
			for (const ESMCharacterType& Type : { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass })
			{
				CharacterTypesMap.Add(Type, TArray<ASMCharacterSelectPlayerState*>());
			}
			return CharacterTypesMap;
		};

		TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>> EDMCharacterTypesMap = InitializeCharacterTypeMap();
		TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>> FBCharacterTypesMap = InitializeCharacterTypeMap();
		TArray EDMAvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };
		TArray FBAvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };

		TArray<ASMCharacterSelectPlayerState*> EDMDuplicatedPlayers;
		TArray<ASMCharacterSelectPlayerState*> FBDuplicatedPlayers;

		// 중복된 플레이어 찾기
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() == ESMCharacterType::None)
				{
					(CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM
						 ? EDMDuplicatedPlayers
						 : FBDuplicatedPlayers).Add(CharacterSelectPlayerState);
					continue;
				}

				TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>>& TargetMap = (CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM) ? EDMCharacterTypesMap : FBCharacterTypesMap;
				TArray<ESMCharacterType>& AvailableTypes = (CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM) ? EDMAvailableCharacterTypes : FBAvailableCharacterTypes;

				TargetMap[CharacterSelectPlayerState->GetCharacterType()].Add(CharacterSelectPlayerState);
				AvailableTypes.Remove(CharacterSelectPlayerState->GetCharacterType());
			}
		}

		for (const auto& [CharacterType, Players] : EDMCharacterTypesMap)
		{
			if (Players.Num() > 1)
			{
				EDMDuplicatedPlayers.Append(Players);
				EDMAvailableCharacterTypes.AddUnique(CharacterType);
				NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Warning, TEXT("EDM Character Type [%s] has duplicated players."), *UEnum::GetValueAsString(CharacterType))
			}
		}

		for (const auto& [CharacterType, Players] : FBCharacterTypesMap)
		{
			if (Players.Num() > 1)
			{
				FBDuplicatedPlayers.Append(Players);
				FBAvailableCharacterTypes.AddUnique(CharacterType);
				NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Warning, TEXT("FB Character Type [%s] has duplicated players."), *UEnum::GetValueAsString(CharacterType))
			}
		}

		EDMDuplicatedPlayers.Sort([](const ASMCharacterSelectPlayerState& A, const ASMCharacterSelectPlayerState& B) {
			return FMath::RandBool();
		});

		FBDuplicatedPlayers.Sort([](const ASMCharacterSelectPlayerState& A, const ASMCharacterSelectPlayerState& B) {
			return FMath::RandBool();
		});

		for (int32 i = 0; i < EDMDuplicatedPlayers.Num(); i++)
		{
			if (i >= EDMAvailableCharacterTypes.Num())
			{
				EDMDuplicatedPlayers[i]->ChangeCharacterType(ESMCharacterType::ElectricGuitar);
				continue;
			}
			EDMDuplicatedPlayers[i]->ChangeCharacterType(EDMAvailableCharacterTypes[i]);
			NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Warning, TEXT("Player %s automatically changed to %s"), *EDMDuplicatedPlayers[i]->GetPlayerName(), *UEnum::GetValueAsString(EDMAvailableCharacterTypes[i]))
		}

		for (int32 i = 0; i < FBDuplicatedPlayers.Num(); i++)
		{
			if (i >= FBAvailableCharacterTypes.Num())
			{
				FBDuplicatedPlayers[i]->ChangeCharacterType(ESMCharacterType::ElectricGuitar);
				continue;
			}
			FBDuplicatedPlayers[i]->ChangeCharacterType(FBAvailableCharacterTypes[i]);
			NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Warning, TEXT("Player %s automatically changed to %s"), *FBDuplicatedPlayers[i]->GetPlayerName(), *UEnum::GetValueAsString(FBAvailableCharacterTypes[i]))
		}
	}
}

void ASMCharacterSelectMode::StartGame()
{
	// const UWorld* World = GetWorld();
	// const USMGameInstance* GameInstance = World ? World->GetGameInstance<USMGameInstance>() : nullptr;
	// const TArray<FSoftObjectPath>& StageLevels = GameInstance ? GameInstance->GetStageLevels() : TArray<FSoftObjectPath>();
	// if (StageLevels.IsValidIndex(0))
	// {
	// 	ProcessServerTravel(StageLevels[0].GetLongPackageName());
	// }

	if (USMMatchSubsystem* Match = USMMatchSubsystem::Get(this))
	{
		if (const USMMatchDefinition* MatchDefinition = Match->GetMatchDefinition())
		{
			FString NextUrl = MatchDefinition->GetRoundUrl(1);
			ProcessServerTravel(NextUrl, false);

			NET_LOG_CATEGORY(this, LogStereoMixCharacterSelect, Log, TEXT("Seamless traveling to %s"), *NextUrl);
		}
	}
}
