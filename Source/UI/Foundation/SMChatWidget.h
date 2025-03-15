// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "Chat/SMChatComponent.h"
#include "UI/Common/SMActivatableWidget.h"

#include "SMChatWidget.generated.h"

class ASMPlayerController;
struct FSMChatMessage;
class UCommonListView;
class UEditableTextBox;

UENUM(BlueprintType)
enum class ESMChatType : uint8
{
	All,
	Team
};

/**
 * StereoMix Chat Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMChatWidget : public USMActivatableWidget
{
	GENERATED_BODY()

public:
	USMChatWidget();

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	virtual FReply NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent) override;

	/** 채팅 위젯을 엽니다. */
	UFUNCTION(BlueprintCallable)
	virtual void OpenChat();

	UFUNCTION(BlueprintCallable)
	void SendMessage();

	UFUNCTION(BlueprintImplementableEvent, Category = Chat, meta = (DisplayName = "On Submit Message"))
	void BP_OnSubmitMessage(const FText& Message);
	virtual void NativeOnSubmitMessage(const FText& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = Chat, meta = (DisplayName = "On Receive Message"))
	void BP_OnReceiveMessage(const FSMChatFromInfo& FromInfo, bool bIsTeamMessage, const FString& Message);
	virtual void NativeOnReceiveMessage(const FSMChatFromInfo& FromInfo, bool bIsTeamMessage, const FString& Message);

	/** 채팅 타입 변경 가능 여부를 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "Chat")
	bool IsChatTypeChangeable() const { return bChatTypeChangeable; }

	/** 채팅 타입 변경 가능 여부를 설정합니다. 변경 가능으로 설정 시 현재 채팅 타입이 'All'이 아닌 경우 'All'로 변경합니다. */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetChatTypeChangeable(bool bNewChatTypeChangeable);

	/** 현재 활성화된 채팅 타입을 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "Chat")
	ESMChatType GetActiveChatType() const { return ActiveChatType; }

	/** 현재 활성화된 채팅 타입을 설정합니다. bChatTypeChangeable = false인 경우 변경되지 않습니다. */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetActiveChatType(ESMChatType NewChatType);

	/** 현재 활성화된 채팅 타입을 다음 채팅 타입으로 변경합니다. bChatTypeChangeable = false인 경우 변경되지 않습니다. */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetActiveChatTypeToNext();

	UFUNCTION(BlueprintImplementableEvent, Category = Chat, meta = (DisplayName = "On Chat Type Changed"))
	void BP_OnChatTypeChanged(ESMChatType NewChatType);
	virtual void NativeOnChatTypeChanged(ESMChatType NewChatType);

	/** 메시지를 작성한 이후 호출됩니다. true를 반환하면 다음 네이티브 로직이 수행되지 않습니다. */
	UFUNCTION(BlueprintImplementableEvent, Category = Chat, meta = (DisplayName = "On Message Committed"))
	bool BP_OnMessageCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	virtual void NativeOnMessageCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChatMessageReceived(FGameplayTag Tag, const FSMChatMessage& Message);

	/** 채팅 메시지를 작성한 이후 위젯을 비활성화할 지 여부를 정합니다. */
	UPROPERTY(EditAnywhere, Category = "Chat")
	bool bDeactivateAfterSendMessage = true;

	/** 채팅 타입을 변경할 때 사용할 트리거 키 문자입니다. */
	UPROPERTY(EditAnywhere, Category = "Chat")
	uint8 ChatTypeChangeKeyChar = '\t';

	UPROPERTY(EditDefaultsOnly, Category = "Chat", meta = (DesignerRebuild))
	TArray<FWidgetChild> VisibleWidgetsOnActivated;

	UPROPERTY(EditDefaultsOnly, Category = "Chat", meta = (DesignerRebuild))
	TArray<FWidgetChild> VisibleWidgetsOnDeactivated;

private:
	void InitActivatableWidgets();

	static void SetVisibilityForActivatableWidgets(const TArray<FWidgetChild>& Widgets, ESlateVisibility InVisibility);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UEditableTextBox> MessageInputBox;

	/** 채팅 타입 변경 여부를 정합니다. */
	UPROPERTY(VisibleInstanceOnly, Category = "Chat")
	bool bChatTypeChangeable = true;

	/** 채팅 타입을 나타냅니다. */
	UPROPERTY(VisibleInstanceOnly, Category = "Chat")
	ESMChatType ActiveChatType = ESMChatType::All;

	UPROPERTY()
	TWeakObjectPtr<ASMPlayerController> OwningPlayerController;

	UPROPERTY()
	TWeakObjectPtr<USMChatComponent> ChatComponent;

	FGameplayMessageListenerHandle ChatListenerHandler;
};
