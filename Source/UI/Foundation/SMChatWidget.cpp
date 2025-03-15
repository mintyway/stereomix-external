// Copyright Studio Surround. All Rights Reserved.

#include "SMChatWidget.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "CommonListView.h"
#include "CommonUILibrary.h"
#include "Components/EditableTextBox.h"
#include "Player/SMPlayerController.h"
#include "NativeGameplayTags.h"
#include "Chat/SMChatMessage.h"
#include "UI/SMUIActionRouter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMChatWidget)

UE_DEFINE_GAMEPLAY_TAG_STATIC(GameplayMessage_Chat, "GameplayMessage.Chat");

USMChatWidget::USMChatWidget()
{
	bSetVisibilityOnActivated = true;
	bSetVisibilityOnDeactivated = true;
	ActivatedVisibility = ESlateVisibility::SelfHitTestInvisible;
	DeactivatedVisibility = ESlateVisibility::HitTestInvisible;

	InputConfig = ESMWidgetInputMode::Menu;
	GameMouseCaptureMode = EMouseCaptureMode::NoCapture;
}

void USMChatWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	MessageInputBox->OnTextCommitted.AddUniqueDynamic(this, &USMChatWidget::USMChatWidget::NativeOnMessageCommitted);
}

void USMChatWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	InitActivatableWidgets();
}

void USMChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// activate일 때 visibility가 바뀌는 위젯들은 먼저 Collapsed로 설정한 후 Activate 시 Visible로 변경
	// deactivate일 때 visibility가 바뀌는 위젯들은 먼저 Visible로 설정한 후 Activate 시 Collapsed로 변경
	SetVisibilityForActivatableWidgets(VisibleWidgetsOnActivated, ESlateVisibility::Collapsed);
	SetVisibilityForActivatableWidgets(VisibleWidgetsOnDeactivated, ESlateVisibility::Visible);

	MessageInputBox->SetText(FText::GetEmpty());

	if (ASMPlayerController* PlayerController = GetOwningPlayer<ASMPlayerController>())
	{
		OwningPlayerController = PlayerController;
		ChatComponent = PlayerController->FindComponentByClass<USMChatComponent>();

		UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
		ChatListenerHandler = GameplayMessageSubsystem.RegisterListener(GameplayMessage_Chat, this, &ThisClass::OnChatMessageReceived);
	}
}

void USMChatWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	GameplayMessageSubsystem.UnregisterListener(ChatListenerHandler);
}

void USMChatWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	SetVisibilityForActivatableWidgets(VisibleWidgetsOnActivated, ESlateVisibility::Visible);
	SetVisibilityForActivatableWidgets(VisibleWidgetsOnDeactivated, ESlateVisibility::Collapsed);
}

void USMChatWidget::NativeOnDeactivated()
{
	MessageInputBox->SetText(FText::GetEmpty());
	SetVisibilityForActivatableWidgets(VisibleWidgetsOnActivated, ESlateVisibility::Collapsed);
	SetVisibilityForActivatableWidgets(VisibleWidgetsOnDeactivated, ESlateVisibility::Visible);

	Super::NativeOnDeactivated();

	if (UCommonUIActionRouterBase* ActionRouter = UCommonUIActionRouterBase::Get(*this))
	{
		if (USMPrimaryLayout* PrimaryLayout = Cast<USMPrimaryLayout>(UCommonUILibrary::FindParentWidgetOfType(this, USMPrimaryLayout::StaticClass())))
		{
			ActionRouter->SetActiveUIInputConfig(PrimaryLayout->GetDesiredInputConfig().GetValue());
		}
	}
}

UWidget* USMChatWidget::NativeGetDesiredFocusTarget() const
{
	return MessageInputBox;
}

FReply USMChatWidget::NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent)
{
	if (InCharEvent.GetCharacter() == ChatTypeChangeKeyChar && !InCharEvent.IsRepeat())
	{
		SetActiveChatTypeToNext();
		return FReply::Handled();
	}
	return Super::NativeOnKeyChar(InGeometry, InCharEvent);
}

void USMChatWidget::OpenChat()
{
	if (!IsActivated())
	{
		ActivateWidget();
	}
	else
	{
		MessageInputBox->SetKeyboardFocus();
	}
}

void USMChatWidget::SendMessage()
{
	if (IsActivated())
	{
		FText Message = MessageInputBox->GetText();
		if (!Message.IsEmpty())
		{
			NativeOnSubmitMessage(Message);
			MessageInputBox->SetText(FText::GetEmpty());
		}
	}
}

void USMChatWidget::NativeOnSubmitMessage(const FText& Message)
{
	BP_OnSubmitMessage(Message);
	if (ChatComponent.IsValid())
	{
		ChatComponent->ServerSendMessage(ActiveChatType == ESMChatType::Team, Message.ToString());
	}
}

void USMChatWidget::NativeOnReceiveMessage(const FSMChatFromInfo& FromInfo, bool bIsTeamMessage, const FString& Message)
{
	BP_OnReceiveMessage(FromInfo, bIsTeamMessage, Message);
}

void USMChatWidget::SetChatTypeChangeable(bool bNewChatTypeChangeable)
{
	if (bNewChatTypeChangeable != bChatTypeChangeable)
	{
		bChatTypeChangeable = bNewChatTypeChangeable;
		if (!bChatTypeChangeable && GetActiveChatType() != ESMChatType::All)
		{
			SetActiveChatType(ESMChatType::All);
		}
	}
}

void USMChatWidget::SetActiveChatType(ESMChatType NewChatType)
{
	if (bChatTypeChangeable && ActiveChatType != NewChatType)
	{
		ActiveChatType = NewChatType;
		NativeOnChatTypeChanged(ActiveChatType);
	}
}

void USMChatWidget::SetActiveChatTypeToNext()
{
	if (bChatTypeChangeable)
	{
		switch (ActiveChatType)
		{
			case ESMChatType::All:
				SetActiveChatType(ESMChatType::Team);
				break;
			case ESMChatType::Team:
				SetActiveChatType(ESMChatType::All);
				break;
		}
	}
}

void USMChatWidget::NativeOnChatTypeChanged(ESMChatType NewChatType)
{
	BP_OnChatTypeChanged(NewChatType);
}

void USMChatWidget::NativeOnMessageCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsActivated())
	{
		return;
	}
	if (Text.IsEmpty())
	{
		DeactivateWidget();
		return;
	}

	if (BP_OnMessageCommited(Text, CommitMethod))
	{
		return;
	}
	if (CommitMethod == ETextCommit::Type::OnCleared)
	{
		DeactivateWidget();
		return;
	}

	if (IsActivated() && CommitMethod == ETextCommit::OnEnter)
	{
		SendMessage();
	}

	if (bDeactivateAfterSendMessage)
	{
		DeactivateWidget();
	}
	else
	{
		MessageInputBox->SetText(FText::GetEmpty());
		MessageInputBox->SetKeyboardFocus();
	}
}

void USMChatWidget::OnChatMessageReceived(FGameplayTag Tag, const FSMChatMessage& Message)
{
	NativeOnReceiveMessage(Message.FromInfo, Message.bIsTeamMessage, Message.Message);
}

void USMChatWidget::InitActivatableWidgets()
{
	for (FWidgetChild& TargetWidget : VisibleWidgetsOnActivated)
	{
		TargetWidget.Resolve(WidgetTree);
	}
	for (FWidgetChild& TargetWidget : VisibleWidgetsOnDeactivated)
	{
		TargetWidget.Resolve(WidgetTree);
	}
}

void USMChatWidget::SetVisibilityForActivatableWidgets(const TArray<FWidgetChild>& Widgets, ESlateVisibility InVisibility)
{
	for (const FWidgetChild& TargetWidget : Widgets)
	{
		if (UWidget* Widget = TargetWidget.GetWidget())
		{
			Widget->SetVisibility(InVisibility);
		}
	}
}
