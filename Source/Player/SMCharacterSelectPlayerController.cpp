// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerController.h"

#include "EngineUtils.h"
#include "Actors/Character/SMPreviewCharacter.h"
#include "Games/CharacterSelect/SMCharacterSelectMode.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "UI/CharacterSelector/SMCharacterSelectorScreenWidget.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectPlayerController::ASMCharacterSelectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;

	bAutoInitializeUserInterface = false;
}

void ASMCharacterSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASMCharacterSelectPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASMCharacterSelectPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetWorld()->GetGameState())
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ASMCharacterSelectPlayerController::InitPlayer);
	}
	else
	{
		NET_LOG(this, Verbose, TEXT("GameState is not set yet. Register GameStateSetEvent."))
		GetWorld()->GameStateSetEvent.AddLambda([this](AGameStateBase*) { InitPlayer(); });
	}
}

void ASMCharacterSelectPlayerController::RequestImmediateStartGame_Implementation()
{
#if WITH_SERVER_CODE
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		ASMCharacterSelectMode* CharacterSelectMode = CastChecked<ASMCharacterSelectMode>(GetWorld()->GetAuthGameMode());
		CharacterSelectMode->ImmediateStart();
	}
#endif
}

void ASMCharacterSelectPlayerController::InitPlayer()
{
	CharacterSelectState = GetWorld()->GetGameStateChecked<ASMCharacterSelectState>();
	CharacterSelectState->OnCurrentStateChanged.Clear();
	CharacterSelectState->OnCurrentStateChanged.AddDynamic(this, &ASMCharacterSelectPlayerController::OnCurrentCharacterSelectStateChanged);

	CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
	CharacterSelectPlayerState->OnCharacterChangeResponse.Clear();
	CharacterSelectPlayerState->OnCharacterChangeResponse.AddDynamic(this, &ASMCharacterSelectPlayerController::OnCharacterChangeResponse);

	ESMTeam Team = CharacterSelectPlayerState->GetTeam();
	for (TActorIterator<AActor> It(GetWorld(), ASMPreviewCharacter::StaticClass()); It; ++It)
	{
		if (ASMPreviewCharacter* TargetCharacter = Cast<ASMPreviewCharacter>(*It); TargetCharacter && TargetCharacter->GetTeam() == Team)
		{
			PreviewCharacters.Add(TargetCharacter->GetCharacterType(), TargetCharacter);
		}
	}

	CharacterSelectPlayerState->SetCurrentState(ECharacterSelectPlayerStateType::Unready);

	// 로딩이 엄청 걸려서 늦게 초기화 완료된 경우 직접 호출
	if (CharacterSelectState->GetCurrentState() != ECharacterSelectionStateType::Wait)
	{
		OnCurrentCharacterSelectStateChanged(CharacterSelectState->GetCurrentState());
	}
}

void ASMCharacterSelectPlayerController::OnCurrentCharacterSelectStateChanged(ECharacterSelectionStateType NewCharacterSelectionState)
{
	if (NewCharacterSelectionState == ECharacterSelectionStateType::Select && !CharacterSelectWidget)
	{
		USMCharacterSelectorScreenWidget* CharacterSelectorScreenWidget = Cast<USMCharacterSelectorScreenWidget>(InitializeUserInterface());
		if (ensure(CharacterSelectorScreenWidget))
		{
			CharacterSelectWidget = CharacterSelectorScreenWidget;
			CharacterSelectWidget->InitWidget(this, CharacterSelectState.Get(), CharacterSelectPlayerState.Get());
		}
	}
}

void ASMCharacterSelectPlayerController::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (bSuccess)
	{
		if (const TObjectPtr<ASMPreviewCharacter>* PreviewCharacter = PreviewCharacters.Find(NewCharacterType))
		{
			PreviewCharacter->Get()->PlaySelectAnimation();
		}
	}
}
