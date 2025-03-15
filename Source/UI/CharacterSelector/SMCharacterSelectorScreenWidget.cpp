// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorScreenWidget.h"

#include "CommonTextBlock.h"
#include "EngineUtils.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "SMCharacterSelectorInformationWidget.h"
#include "SMCharacterSelectorTimerWidget.h"
#include "SMCharacterSelectorTypeDisplayWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/Core/SMCountdownTimerComponent.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "Internationalization/StringTableRegistry.h"
#include "Player/SMCharacterSelectPlayerController.h"


void USMCharacterSelectorScreenWidget::InitWidget(ASMCharacterSelectPlayerController* PlayerController, ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState)
{
	if (ensure(PlayerController))
	{
		OwningPlayerController = PlayerController;
	}

	if (ensure(CharacterSelectState))
	{
		OwningCharacterSelectState = CharacterSelectState;
	}

	if (ensure(PlayerState))
	{
		OwningPlayerState = PlayerState;
	}

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (CharacterSelectorInformationWidget == nullptr)
		{
			CharacterSelectorInformationWidget = Cast<USMCharacterSelectorInformationWidget>(Widget);
		}

		if (CharacterSelectorTypeDisplayWidget == nullptr)
		{
			CharacterSelectorTypeDisplayWidget = Cast<USMCharacterSelectorTypeDisplayWidget>(Widget);
		}

		if (CharacterSelectorTimerWidget == nullptr)
		{
			CharacterSelectorTimerWidget = Cast<USMCharacterSelectorTimerWidget>(Widget);
		}

		if (CharacterSelectorInformationWidget.IsValid() && CharacterSelectorTypeDisplayWidget.IsValid() && CharacterSelectorTimerWidget.IsValid())
		{
			break;
		}
	}

	for (TObjectPtr<APlayerState> Player : GetOwningCharacterSelectState()->PlayerArray)
	{
		if (ASMPlayerState* TargetPlayerState = Cast<ASMPlayerState>(Player))
		{
			if (TargetPlayerState->GetTeam() == GetPlayerTeam())
			{
				CurrentTeamPlayers.AddUnique(TargetPlayerState);
			}
		}
	}

	if (GetPlayerTeam() == ESMTeam::EDM)
	{
		FBElectricGuitar->SetVisibility(ESlateVisibility::Hidden);
		FBPiano->SetVisibility(ESlateVisibility::Hidden);
		FBBass->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		EDMElectricGuitar->SetVisibility(ESlateVisibility::Hidden);
		EDMPiano->SetVisibility(ESlateVisibility::Hidden);
		EDMBass->SetVisibility(ESlateVisibility::Hidden);
	}

	OriginalButtonStyle = PickElectricGuitar->GetStyle();
	OriginalButtonStyle.Normal = PickElectricGuitar->GetStyle().Normal;
	OriginalButtonStyle.Normal.ImageSize = PickElectricGuitar->GetStyle().Normal.ImageSize;
	OriginalButtonStyle.Pressed = PickElectricGuitar->GetStyle().Hovered;
	OriginalButtonStyle.Pressed.ImageSize = PickElectricGuitar->GetStyle().Hovered.ImageSize;

	OriginalFocusedButtonStyle = PickElectricGuitar->GetStyle();
	OriginalFocusedButtonStyle.Normal = PickElectricGuitar->GetStyle().Pressed;
	OriginalFocusedButtonStyle.Normal.ImageSize = PickElectricGuitar->GetStyle().Pressed.ImageSize;
	OriginalFocusedButtonStyle.Pressed = PickElectricGuitar->GetStyle().Hovered;
	OriginalFocusedButtonStyle.Pressed.ImageSize = PickElectricGuitar->GetStyle().Hovered.ImageSize;

	OriginalSelectedButtonStyle = PickElectricGuitar->GetStyle();
	OriginalSelectedButtonStyle.Normal = PickElectricGuitar->GetStyle().Disabled;
	OriginalSelectedButtonStyle.Normal.ImageSize = PickElectricGuitar->GetStyle().Disabled.ImageSize;
	OriginalSelectedButtonStyle.Pressed = PickElectricGuitar->GetStyle().Hovered;
	OriginalSelectedButtonStyle.Pressed.ImageSize = PickElectricGuitar->GetStyle().Hovered.ImageSize;

	OriginalDisabledButtonStyle = PickElectricGuitar->GetStyle();
	OriginalDisabledButtonStyle.Normal = PickElectricGuitar->GetStyle().Normal;
	OriginalDisabledButtonStyle.Normal.ImageSize = PickElectricGuitar->GetStyle().Normal.ImageSize;
	OriginalDisabledButtonStyle.Pressed = PickElectricGuitar->GetStyle().Hovered;
	OriginalDisabledButtonStyle.Pressed.ImageSize = PickElectricGuitar->GetStyle().Hovered.ImageSize;

	PickElectricGuitar->OnClicked.AddDynamic(this, &ThisClass::OnPickElectricGuitar);
	PickPiano->OnClicked.AddDynamic(this, &ThisClass::OnPickPiano);
	PickBass->OnClicked.AddDynamic(this, &ThisClass::OnPickBass);

	SelectButton->OnClicked.AddDynamic(this, &ThisClass::OnSelectButtonClicked);

	OwningCharacterSelectState->OnPlayerAddedEvent.AddUObject(this, &ThisClass::OnPlayerJoin);
	OwningCharacterSelectState->OnPlayerRemovedEvent.AddUObject(this, &ThisClass::OnPlayerLeft);

	for (TObjectPtr<APlayerState>& Player : OwningCharacterSelectState->PlayerArray)
	{
		ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(Player);
		TargetPlayerState->GetOnCharacterChangedDelegateChecked().AddUObject(this, &ThisClass::OnPlayerCharacterChanged);
	}

	OwningCharacterSelectState->OnPlayerCharacterFocusChanged.AddDynamic(this, &ThisClass::OnPlayerCharacterFocusChanged);

	OwningPlayerState->OnCharacterChangeResponse.AddDynamic(this, &ThisClass::OnCharacterChangeResponse);

	OwningCharacterSelectState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ThisClass::OnCountdownTick);
	OwningCharacterSelectState->OnCurrentStateChanged.AddDynamic(this, &ThisClass::OnCurrentStateChanged);

	UpdatePlayerList();
}

void USMCharacterSelectorScreenWidget::OnCountdownTick()
{
	if (CharacterSelectorTimerWidget.IsValid())
	{
		const int32 RemainingTime = OwningCharacterSelectState->GetCountdownTimer()->GetRemainingTime();
		CharacterSelectorTimerWidget->SetRemainTime(RemainingTime);
	}
}

void USMCharacterSelectorScreenWidget::OnCurrentStateChanged(ECharacterSelectionStateType NewCharacterSelectionState)
{
	if (NewCharacterSelectionState == ECharacterSelectionStateType::End)
	{
		bIsSelectStateEnd = true;
		SetCharacterSelectScreenVisibility(false);

		if (FocusedCharacterType != OwningPlayerState->GetCharacterType())
		{
			ChangeFocusedCharacter(OwningPlayerState->GetCharacterType());
		}

		if (CharacterSelectorTypeDisplayWidget.IsValid())
		{
			CharacterSelectorTypeDisplayWidget->SetReady(true);
		}

		bSelectEnabled = false;

		UE_LOG(LogTemp, Warning, TEXT("Character Selection State: End"));
	}
}

void USMCharacterSelectorScreenWidget::OnPickElectricGuitar()
{
	ChangeFocusedCharacter(ESMCharacterType::ElectricGuitar);
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	SetPlayerReady(GetOwningPlayerState(), ESMCharacterType::ElectricGuitar, false, true);
}

void USMCharacterSelectorScreenWidget::OnPickPiano()
{
	ChangeFocusedCharacter(ESMCharacterType::Piano);
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	SetPlayerReady(GetOwningPlayerState(), ESMCharacterType::Piano, false, true);
}

void USMCharacterSelectorScreenWidget::OnPickBass()
{
	ChangeFocusedCharacter(ESMCharacterType::Bass);
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	SetPlayerReady(GetOwningPlayerState(), ESMCharacterType::Bass, false, true);
}

void USMCharacterSelectorScreenWidget::OnSelectButtonClicked()
{
	bIsSelectButtonClicked = !bIsSelectButtonClicked;

	if (!bIsSelectButtonClicked)
	{
		ElectricGuitarSelectFX->SetVisibility(ESlateVisibility::Hidden);
		PianoSelectFX->SetVisibility(ESlateVisibility::Hidden);
		BassSelectFX->SetVisibility(ESlateVisibility::Hidden);

		switch (FocusedCharacterType)
		{
			case ESMCharacterType::None:
				break;
			case ESMCharacterType::ElectricGuitar:
				PickElectricGuitar->SetStyle(OriginalFocusedButtonStyle);
				break;
			case ESMCharacterType::Piano:
				PickPiano->SetStyle(OriginalFocusedButtonStyle);
				break;
			case ESMCharacterType::Bass:
				PickBass->SetStyle(OriginalFocusedButtonStyle);
				break;
		}
		SelectButtonText->SetText(LOCTABLE("UI", "Select"));

		SetPlayerReady(GetOwningPlayerState(), FocusedCharacterType, false, true);
		GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
		return;
	}

	if (IsFocusedCharacterSelectable(true))
	{
		for (const TObjectPtr<UImage>& PickFX : { PickUpFX1, PickDownFX1, PickUpFX2, PickDownFX2, PickUpFX3, PickDownFX3 })
		{
			if (PickFX && PickFX->GetDynamicMaterial())
			{
				const float RandomScalar = FMath::RandRange(0.0f, 1.0f);
				PickFX->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Seed"), RandomScalar);
			}
		}
		ElectricGuitarSelectFX->SetVisibility(FocusedCharacterType == ESMCharacterType::ElectricGuitar ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		PianoSelectFX->SetVisibility(FocusedCharacterType == ESMCharacterType::Piano ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		BassSelectFX->SetVisibility(FocusedCharacterType == ESMCharacterType::Bass ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		switch (FocusedCharacterType)
		{
			case ESMCharacterType::None:
				break;
			case ESMCharacterType::ElectricGuitar:
				PickElectricGuitar->SetStyle(OriginalSelectedButtonStyle);
				break;
			case ESMCharacterType::Piano:
				PickPiano->SetStyle(OriginalSelectedButtonStyle);
				break;
			case ESMCharacterType::Bass:
				PickBass->SetStyle(OriginalSelectedButtonStyle);
				break;
		}
		SelectButtonText->SetText(LOCTABLE("UI", "Cancel"));

		SetPlayerReady(GetOwningPlayerState(), FocusedCharacterType, true, true);
		GetOwningPlayerState()->ChangeCharacterType(FocusedCharacterType);

		OnCharacterChanged.Broadcast(FocusedCharacterType);
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerJoin(ASMPlayerState* JoinedPlayer)
{
	JoinedPlayer->GetOnCharacterChangedDelegateChecked().AddUObject(this, &ThisClass::OnPlayerCharacterChanged);
	if (JoinedPlayer->GetTeam() == GetPlayerTeam())
	{
		CurrentTeamPlayers.AddUnique(JoinedPlayer);
		UpdatePlayerList();
		UpdateSelectButton();
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	SetPlayerReady(LeftPlayer, ESMCharacterType::None, false);
	CurrentTeamPlayers.Remove(LeftPlayer);
	UpdatePlayerList();
	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPlayerCharacterChanged(UObject* ObjectChangingCharacter, ESMCharacterType OldCharacterType, ESMCharacterType NewCharacterType)
{
	ASMCharacterSelectPlayerState* Player = Cast<ASMCharacterSelectPlayerState>(ObjectChangingCharacter);
	if (!Player)
	{
		return;
	}

	if (Player->GetTeam() != GetPlayerTeam())
	{
		return;
	}

	const bool IsOwningPlayer = (Player == GetOwningPlayerState());
	const bool IsValidCharacter = (NewCharacterType != ESMCharacterType::None);
	const bool IsDifferentCharacter = (FocusedCharacterType != NewCharacterType);

	UpdatePlayerList();
	UpdateSelectButton();

	if (IsOwningPlayer && IsValidCharacter)
	{
		if (CharacterSelectorTypeDisplayWidget.IsValid())
		{
			CharacterSelectorTypeDisplayWidget->SetReady(true);
		}
	}

	if (IsOwningPlayer && IsValidCharacter && IsDifferentCharacter)
	{
		FocusedCharacterType = NewCharacterType;
		GetOwningPlayerState()->SetPredictFocusCharacterType(FocusedCharacterType);
		GetOwningPlayerState()->ChangeFocusCharacterType(FocusedCharacterType);
		ShowPreviewCharacter(FocusedCharacterType);
		UpdatePlayerList();
		if (CharacterSelectorInformationWidget.IsValid())
		{
			CharacterSelectorInformationWidget->SetSkillInfo(FocusedCharacterType);
		}
		if (CharacterSelectorTypeDisplayWidget.IsValid())
		{
			CharacterSelectorTypeDisplayWidget->SetCharacterType(FocusedCharacterType);
		}
	}
}

void USMCharacterSelectorScreenWidget::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (!bSuccess)
	{
		GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerCharacterFocusChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	UpdatePlayerList();
	UpdateSelectButton();
}

TArray<TObjectPtr<ASMPlayerState>> USMCharacterSelectorScreenWidget::GetTeamPlayersSortedById() const
{
	TArray<TObjectPtr<ASMPlayerState>> PlayerArray = CurrentTeamPlayers;
	PlayerArray.Sort([](const APlayerState& A, const APlayerState& B) {
		return A.GetPlayerId() < B.GetPlayerId();
	});

	return PlayerArray;
}

ESMCharacterType USMCharacterSelectorScreenWidget::GetRandomCharacter() const
{
	TArray AvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };

	const TArray<TObjectPtr<ASMPlayerState>> TeamPlayersSortedById = GetTeamPlayersSortedById();
	ASMCharacterSelectPlayerState* CachedPlayerState = GetOwningPlayerState();
	const int32 FixedIndex = TeamPlayersSortedById.IndexOfByPredicate([CachedPlayerState](const TObjectPtr<ASMPlayerState>& Player) {
		return Player == CachedPlayerState;
	});

	// Todo: 크로니클 용 캐릭터 선택 시퀀스 중복 방지
	if (FixedIndex != INDEX_NONE && FixedIndex < AvailableCharacterTypes.Num())
	{
		return AvailableCharacterTypes[FixedIndex];
	}

	return AvailableCharacterTypes[FMath::RandRange(0, AvailableCharacterTypes.Num() - 1)];
}

ESMTeam USMCharacterSelectorScreenWidget::GetPlayerTeam() const
{
	return GetOwningPlayerState()->GetTeam();
}

void USMCharacterSelectorScreenWidget::ChangeFocusedCharacter(const ESMCharacterType CharacterType)
{
	if (!bSelectEnabled || FocusedCharacterType == CharacterType)
	{
		return;
	}

	FocusedCharacterType = CharacterType;
	GetOwningPlayerState()->SetPredictFocusCharacterType(CharacterType);
	GetOwningPlayerState()->ChangeFocusCharacterType(CharacterType);

	UpdatePlayerList();
	UpdateSelectButton();

	ShowPreviewCharacter(FocusedCharacterType);
	if (CharacterSelectorInformationWidget.IsValid())
	{
		CharacterSelectorInformationWidget->SetSkillInfo(FocusedCharacterType);
	}
	if (CharacterSelectorTypeDisplayWidget.IsValid())
	{
		CharacterSelectorTypeDisplayWidget->SetCharacterType(FocusedCharacterType);
	}

	OnCharacterFocusChanged.Broadcast(FocusedCharacterType);
}

void USMCharacterSelectorScreenWidget::ShowPreviewCharacter(const ESMCharacterType CharacterType)
{
	const ESMTeam LocalTeam = GetPlayerTeam();
	FName TargetTag = TEXT("ABP_None_FB");
	const FName SelectFXTag = LocalTeam == ESMTeam::EDM ? TEXT("EDMCharacterSelectFX") : TEXT("FBCharacterSelectFX");
	switch (CharacterType)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			TargetTag = LocalTeam == ESMTeam::EDM ? TEXT("ElectricGuitar_EDM") : TEXT("ElectricGuitar_FB");
			break;
		case ESMCharacterType::Piano:
			TargetTag = LocalTeam == ESMTeam::EDM ? TEXT("Piano_EDM") : TEXT("Piano_FB");
			break;
		case ESMCharacterType::Bass:
			TargetTag = LocalTeam == ESMTeam::EDM ? TEXT("Bass_EDM") : TEXT("Bass_FB");
			break;
	}

	if (CharacterMesh)
	{
		CharacterMesh->SetActorHiddenInGame(true);
	}

	if (const UWorld* World = GetWorld())
	{
		ANiagaraActor* SelectFX = nullptr;
		for (ANiagaraActor* Actor : TActorRange<ANiagaraActor>(World))
		{
			if (Actor->ActorHasTag(SelectFXTag))
			{
				SelectFX = Actor;
				break;
			}
		}

		if (SelectFX == nullptr)
		{
			return;
		}

		for (AActor* Actor : TActorRange<AActor>(World))
		{
			if (Actor->ActorHasTag(TargetTag))
			{
				CharacterMesh = Actor;
				CharacterMesh->SetActorHiddenInGame(false);
				if (const USkeletalMeshComponent* MeshComponent = CharacterMesh->FindComponentByClass<USkeletalMeshComponent>())
				{
					const FName AttachBoneName = TEXT("b-Spine-0");
					const FVector BoneLocation = MeshComponent->GetBoneLocation(AttachBoneName, static_cast<EBoneSpaces::Type>(RTS_World));
					SelectFX->SetActorLocation(BoneLocation);
					SelectFX->GetNiagaraComponent()->ResetSystem();
					SelectFX->GetNiagaraComponent()->Activate();
				}
				break;
			}
		}
	}
}

void USMCharacterSelectorScreenWidget::SetCharacterSelectScreenVisibility(bool bIsVisible) const
{
	CharacterSelectBox->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void USMCharacterSelectorScreenWidget::UpdatePlayerList() const
{
	const ASMCharacterSelectState* CharacterSelectState = GetOwningCharacterSelectState();
	if (!CharacterSelectState || !CharacterSelectorInformationWidget.IsValid())
	{
		return;
	}

	TArray<ASMCharacterSelectPlayerState*> PlayerArray;

	for (TObjectPtr<APlayerState> PlayerState : GetTeamPlayersSortedById())
	{
		if (ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			PlayerArray.Add(TargetPlayerState);
		}
	}

	if (const int32 PlayerIndex = GetTeamPlayersSortedById().IndexOfByKey(GetOwningPlayerState()); PlayerIndex != INDEX_NONE)
	{
		CharacterSelectorInformationWidget->SetPlayerInfo(PlayerArray, PlayerIndex);
	}
}

void USMCharacterSelectorScreenWidget::UpdateSelectButton()
{
	const bool bIsSelectable = IsFocusedCharacterSelectable(true);
	const FButtonStyle NewButtonStyle = bIsSelectable ? OriginalFocusedButtonStyle : OriginalDisabledButtonStyle;

	PickElectricGuitar->SetStyle(FocusedCharacterType == ESMCharacterType::ElectricGuitar ? NewButtonStyle : OriginalButtonStyle);
	PickPiano->SetStyle(FocusedCharacterType == ESMCharacterType::Piano ? NewButtonStyle : OriginalButtonStyle);
	PickBass->SetStyle(FocusedCharacterType == ESMCharacterType::Bass ? NewButtonStyle : OriginalButtonStyle);

	for (auto& Profile : { EDMElectricGuitar, EDMPiano, EDMBass, FBElectricGuitar, FBPiano, FBBass })
	{
		if (Profile && Profile->GetDynamicMaterial())
		{
			constexpr float EnabledScalar = 0.0f;
			Profile->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), EnabledScalar);
		}
	}

	for (TObjectPtr<ASMPlayerState> Player : GetTeamPlayersSortedById())
	{
		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(Player))
		{
			if (TargetPlayerState == GetOwningPlayerState())
			{
				continue;
			}

			constexpr float SoftDisabledScalar = 1.0f;
			constexpr float DisabledScalar = 2.0f;
			const float SelectedScalar = TargetPlayerState->GetCharacterType() == FocusedCharacterType ? SoftDisabledScalar : DisabledScalar;

			switch (TargetPlayerState->GetCharacterType())
			{
				case ESMCharacterType::ElectricGuitar:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMElectricGuitar : FBElectricGuitar)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), SelectedScalar);
					break;
				case ESMCharacterType::Piano:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMPiano : FBPiano)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), SelectedScalar);
					break;
				case ESMCharacterType::Bass:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMBass : FBBass)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), SelectedScalar);
					break;
				default:
					break;
			}
		}
	}

	const ESlateVisibility SelectFXVisibility = (FocusedCharacterType != ESMCharacterType::None && GetOwningPlayerState()->GetCharacterType() == FocusedCharacterType) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	ElectricGuitarSelectFX->SetVisibility(FocusedCharacterType == ESMCharacterType::ElectricGuitar ? SelectFXVisibility : ESlateVisibility::Hidden);
	PianoSelectFX->SetVisibility(FocusedCharacterType == ESMCharacterType::Piano ? SelectFXVisibility : ESlateVisibility::Hidden);
	BassSelectFX->SetVisibility(FocusedCharacterType == ESMCharacterType::Bass ? SelectFXVisibility : ESlateVisibility::Hidden);

	SelectButtonText->SetText(LOCTABLE("UI", "Select"));

	if (bIsSelectable && GetOwningPlayerState()->GetCharacterType() == FocusedCharacterType)
	{
		switch (FocusedCharacterType)
		{
			case ESMCharacterType::None:
				break;
			case ESMCharacterType::ElectricGuitar:
				PickElectricGuitar->SetStyle(OriginalSelectedButtonStyle);
				break;
			case ESMCharacterType::Piano:
				PickPiano->SetStyle(OriginalSelectedButtonStyle);
				break;
			case ESMCharacterType::Bass:
				PickBass->SetStyle(OriginalSelectedButtonStyle);
				break;
		}
		SelectButtonText->SetText(LOCTABLE("UI", "Cancel"));
	}
	SelectButton->SetIsEnabled(bIsSelectable);

	WarningText->SetVisibility(ESlateVisibility::Hidden);
	if (!bIsSelectable)
	{
		PlayAnimationForward(OnDuplicatedWarning);
	}
}

bool USMCharacterSelectorScreenWidget::IsCharacterSelectable(ESMCharacterType CharacterType, bool bExcludeOwner) const
{
	bool bSelectButtonEnable = true;

	for (TObjectPtr<APlayerState> PlayerState : GetTeamPlayersSortedById())
	{
		if (bExcludeOwner && PlayerState == GetOwningPlayerState())
		{
			continue;
		}

		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			if (TargetPlayerState->GetCharacterType() == CharacterType)
			{
				bSelectButtonEnable = false;
				break;
			}
		}
	}

	return bSelectButtonEnable;
}

bool USMCharacterSelectorScreenWidget::IsFocusedCharacterSelectable(bool bExcludeOwner) const
{
	return IsCharacterSelectable(FocusedCharacterType, bExcludeOwner);
}

void USMCharacterSelectorScreenWidget::SetPlayerReady(ASMPlayerState* Player, const ESMCharacterType CharacterType, const bool bIsReady, const bool bIsPredicated) const
{
	if (CharacterSelectorTypeDisplayWidget.IsValid() && Player == GetOwningPlayerState() && !bIsSelectStateEnd)
	{
		CharacterSelectorTypeDisplayWidget->SetReady(bIsReady);
	}

	if (CharacterSelectorInformationWidget.IsValid())
	{
		const TArray<ASMPlayerState*> PlayerArray = GetTeamPlayersSortedById();
		if (const int32 PlayerIndex = PlayerArray.IndexOfByKey(Player); PlayerIndex != INDEX_NONE)
		{
			CharacterSelectorInformationWidget->SetPlayerReady(PlayerIndex, CharacterType, bIsReady, bIsPredicated);
		}
	}
}
