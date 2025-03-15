// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "UI/Common/SMPrimaryLayout.h"
#include "SMCharacterSelectorScreenWidget.generated.h"

class USMCharacterSelectorTypeDisplayWidget;
class UImage;
class UOverlay;
class USMCharacterSelectorTimerWidget;
class ASkeletalMeshActor;
class ASMCharacterSelectPlayerController;
class USMCharacterSelectorInformationWidget;
class ASMCharacterSelectPlayerState;
class ASMCharacterSelectState;
class UCommonTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterChanged, ESMCharacterType, NewCharacterType);


UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorScreenWidget : public USMPrimaryLayout
{
	GENERATED_BODY()

public:
	void InitWidget(ASMCharacterSelectPlayerController* PlayerController, ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	ASMCharacterSelectState* GetOwningCharacterSelectState() const { return OwningCharacterSelectState.Get(); }

	UFUNCTION(BlueprintCallable)
	ASMCharacterSelectPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

	UFUNCTION(BlueprintCallable)
	ESMCharacterType GetRandomCharacter() const;

	UFUNCTION(BlueprintCallable)
	ESMTeam GetPlayerTeam() const;

	/** 선택한 캐릭터를 변경합니다. (프로필, 정보창, 미리보기 캐릭터 등) */
	UFUNCTION(BlueprintCallable)
	void ChangeFocusedCharacter(ESMCharacterType CharacterType);

	/** 미리보기 캐릭터를 변경합니다. */
	UFUNCTION(BlueprintCallable)
	void ShowPreviewCharacter(ESMCharacterType CharacterType);

	/** 선택한 캐릭터를 변경합니다. */
	UFUNCTION(BlueprintCallable)
	void SetCharacterSelectScreenVisibility(bool bIsVisible) const;

protected:
	UFUNCTION()
	void OnCountdownTick();

	UFUNCTION()
	void OnCurrentStateChanged(ECharacterSelectionStateType NewCharacterSelectionState);

	UFUNCTION()
	void OnPickElectricGuitar();

	UFUNCTION()
	void OnPickPiano();

	UFUNCTION()
	void OnPickBass();

	UFUNCTION()
	void OnSelectButtonClicked();

	UFUNCTION()
	void OnPlayerJoin(ASMPlayerState* JoinedPlayer);

	UFUNCTION()
	void OnPlayerLeft(ASMPlayerState* LeftPlayer);

	UFUNCTION()
	void OnPlayerCharacterChanged(UObject* ObjectChangingCharacter, ESMCharacterType OldCharacterType, ESMCharacterType NewCharacterType);

	UFUNCTION()
	void OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);

	UFUNCTION()
	void OnPlayerCharacterFocusChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter);

	TArray<TObjectPtr<ASMPlayerState>> GetTeamPlayersSortedById() const;

	void UpdatePlayerList() const;

	void UpdateSelectButton();

	UFUNCTION(BlueprintCallable)
	bool IsCharacterSelectable(ESMCharacterType CharacterType, bool bExcludeOwner) const;

	UFUNCTION(BlueprintCallable)
	bool IsFocusedCharacterSelectable(bool bExcludeOwner) const;

	void SetPlayerReady(ASMPlayerState* Player, ESMCharacterType CharacterType, const bool bIsReady, bool bIsPredicated = false) const;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> EDMElectricGuitar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> EDMPiano;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> EDMBass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> FBElectricGuitar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> FBPiano;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> FBBass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> CharacterSelectBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SelectButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> PickElectricGuitar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> PickPiano;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> PickBass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PickUpFX1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PickDownFX1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PickUpFX2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PickDownFX2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PickUpFX3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PickDownFX3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ElectricGuitarSelectFX;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> PianoSelectFX;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> BassSelectFX;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> WarningText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OnDuplicatedWarning;

	TWeakObjectPtr<USMCharacterSelectorInformationWidget> CharacterSelectorInformationWidget;

	TWeakObjectPtr<USMCharacterSelectorTypeDisplayWidget> CharacterSelectorTypeDisplayWidget;

	TWeakObjectPtr<USMCharacterSelectorTimerWidget> CharacterSelectorTimerWidget;

	TWeakObjectPtr<ASMCharacterSelectPlayerController> OwningPlayerController;

	TWeakObjectPtr<ASMCharacterSelectState> OwningCharacterSelectState;

	TWeakObjectPtr<ASMCharacterSelectPlayerState> OwningPlayerState;

	UPROPERTY()
	TObjectPtr<AActor> CharacterMesh;

	FButtonStyle OriginalButtonStyle;

	FButtonStyle OriginalFocusedButtonStyle;

	FButtonStyle OriginalSelectedButtonStyle;

	FButtonStyle OriginalDisabledButtonStyle;

	UPROPERTY(BlueprintReadOnly)
	uint32 bIsSelectStateEnd:1 = false;

	uint32 bSelectEnabled:1 = true;

	uint32 bIsSelectButtonClicked:1 = false;

	UPROPERTY(BlueprintReadOnly)
	ESMCharacterType FocusedCharacterType = ESMCharacterType::None;

	TArray<TObjectPtr<ASMPlayerState>> CurrentTeamPlayers;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterChanged OnCharacterChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterChanged OnCharacterFocusChanged;
};
