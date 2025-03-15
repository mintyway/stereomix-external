// Copyright Studio Surround. All Rights Reserved.


#include "SMFrontendComponent.h"

#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "ControlFlow.h"
#include "PrimaryGameLayout.h"
#include "SMGameplayTags.h"
#include "Audio/SMFMODBlueprintStatics.h"
#include "System/SMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Messaging/CommonGameDialog.h"
#include "Online/SMUserSubsystem.h"
#include "Settings/SMSettingsLocal.h"
#include "UI/Foundation/SMMenuLayout.h"

class IOnlineSubsystemEOS;

using namespace StereoMix;

#define LOCTABLE_ID "Game"

USMFrontendComponent::USMFrontendComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FlowId = TEXT("FrontendFlow");
	DefaultLoadingReason = TEXT("Frontend Flow Pending...");
}

void USMFrontendComponent::MakeFlow(FControlFlow& Flow)
{
	Flow.QueueStep(TEXT("Try Show Splash Screen"), this, &ThisClass::FlowStep_TryShowSplashScreen)
	    .QueueStep(TEXT("Wait for User Initialization"), this, &ThisClass::FlowStep_WaitForUserInitialization)
	    .QueueStep(TEXT("Wait for User Login"), this, &ThisClass::FlowStep_WaitForUserLogin)
	    .QueueStep(TEXT("Try Join Requested Session"), this, &ThisClass::FlowStep_TryJoinRequestedSession)
	    .QueueStep(TEXT("Wait for show Main Screen"), this, &ThisClass::FlowStep_TryShowMainScreen);
}

void USMFrontendComponent::BeginPlay()
{
	Super::BeginPlay();

	USMSettingsLocal::Get()->SetShouldUseFrontendPerformanceSettings(true);
	
	ExecuteFlow();
}

void USMFrontendComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	USMSettingsLocal::Get()->SetShouldUseFrontendPerformanceSettings(false);
	
	if (USMFMODBlueprintStatics::EventInstanceIsValid(BackgroundMusicInstance))
	{
		USMFMODBlueprintStatics::EventInstanceStop(BackgroundMusicInstance, true);
	}
}

void USMFrontendComponent::FlowStep_TryShowSplashScreen(FControlFlowNodeRef SubFlow)
{
	SetShouldShowLoadingScreen(false);

	USMGameInstance* GameInstance = CastChecked<USMGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance->GetGlobalTags().HasTag(Tags::UI_FirstInitialized))
	{
		SubFlow->ContinueFlow();
		return;
	}

	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(Tags::UI_Layer_Menu, true, SplashScreenClass,
			[this, SubFlow](const EAsyncWidgetLayerState State, const UCommonActivatableWidget* Screen) {
				switch (State)
				{
					case EAsyncWidgetLayerState::AfterPush:
						Screen->OnDeactivated().AddWeakLambda(this, [this, SubFlow] {
							USMGameInstance* GameInstance = GetGameInstance<USMGameInstance>();
							GameInstance->GetGlobalTags().AddTag(Tags::UI_FirstInitialized);
							SubFlow->ContinueFlow();
						});
						break;
					case EAsyncWidgetLayerState::Canceled:
						SubFlow->ContinueFlow();
						break;
					default:
						break;
				}
			});
	}
}

void USMFrontendComponent::FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow)
{
	bool bWasHardDisconnect = false;
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);

	if (ensure(GameMode) && UGameplayStatics::HasOption(GameMode->OptionsString, TEXT("closed")))
	{
		bWasHardDisconnect = true;
	}

	// Only reset users on hard disconnect
	USMUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<USMUserSubsystem>();
	if (ensure(UserSubsystem) && bWasHardDisconnect)
	{
		UserSubsystem->ResetUserState();
	}

	// Always reset sessions
	UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
	if (ensure(SessionSubsystem))
	{
		SessionSubsystem->CleanUpSessions();
	}

	SubFlow->ContinueFlow();
}

void USMFrontendComponent::FlowStep_WaitForUserLogin(FControlFlowNodeRef SubFlow)
{
	if (!USMFMODBlueprintStatics::EventInstanceIsValid(BackgroundMusicInstance))
	{
		BackgroundMusicInstance = USMFMODBlueprintStatics::PlayEvent2D(this, BackgroundMusic.LoadSynchronous(), true);
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	USMUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<USMUserSubsystem>();

	// Check to see if the first player is already logged in, if they are, we can skip the press start screen.
	if (const UCommonUserInfo* FirstUser = UserSubsystem->GetUserInfoForLocalPlayerIndex(0))
	{
		if (FirstUser->InitializationState == ECommonUserInitializationState::LoggedInLocalOnly ||
		    FirstUser->InitializationState == ECommonUserInitializationState::LoggedInOnline)
		{
			SubFlow->ContinueFlow();
			return;
		}
	}

	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		RootLayout->PushWidgetToLayerStackAsync<USMMenuLayout>(Tags::UI_Layer_Menu, true, StartScreenClass,
			[this, SubFlow](const EAsyncWidgetLayerState State, const USMMenuLayout* Screen) {
				switch (State)
				{
					case EAsyncWidgetLayerState::AfterPush:
					{
						Screen->OnDeactivated().AddLambda([ThisWeakPtr = TWeakObjectPtr<USMFrontendComponent>(this), SubFlowPtr = SubFlow.ToWeakPtr()] {
							if (ThisWeakPtr.IsValid() && SubFlowPtr.IsValid())
							{
								const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(ThisWeakPtr.Get());
								USMUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<USMUserSubsystem>();
								if (UserSubsystem->GetOnlineSubsystemName() == TEXT("EOS") || UserSubsystem->GetOnlineSubsystemName() == TEXT("EOSPlus"))
								{
									FString IdToken = UserSubsystem->GetStereoMixUserInfoForLocalPlayerIndex(0)->GetToken();
									UE_LOG(LogTemp, Log, TEXT("User IdToken: %s"), *IdToken);
								}
								const TSharedPtr<FControlFlowNode> Flow = SubFlowPtr.Pin();
								Flow->ContinueFlow();
							}
						});
						break;
					}
					case EAsyncWidgetLayerState::Canceled:
						SubFlow->ContinueFlow();
						break;
					default:
						break;
				}
			});
	}
}

void USMFrontendComponent::FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow)
{
	UCommonGameInstance* GameInstance = Cast<UCommonGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (GameInstance->GetRequestedSession() != nullptr && GameInstance->CanJoinRequestedSession())
	{
		UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
		if (ensure(SessionSubsystem))
		{
			// Bind to session join completion to continue or cancel the flow
			// TODO:  Need to ensure that after session join completes, the server travel completes.
			OnJoinSessionCompleteEventHandle = SessionSubsystem->OnJoinSessionCompleteEvent.AddWeakLambda(this, [this, SubFlow, SessionSubsystem](const FOnlineResultInformation& Result) {
				// Unbind delegate. SessionSubsystem is the object triggering this event, so it must still be valid.
				SessionSubsystem->OnJoinSessionCompleteEvent.Remove(OnJoinSessionCompleteEventHandle);
				OnJoinSessionCompleteEventHandle.Reset();

				if (Result.bWasSuccessful)
				{
					// No longer transitioning to the main menu
					SubFlow->CancelFlow();
				}
				else
				{
					// Proceed to the main menu
					SubFlow->ContinueFlow();
					return;
				}
			});
			GameInstance->JoinRequestedSession();
			return;
		}
	}
	// Skip this step if we didn't start requesting a session join
	SubFlow->ContinueFlow();
}

void USMFrontendComponent::FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow)
{
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		RootLayout->PushWidgetToLayerStackAsync<USMMenuLayout>(Tags::UI_Layer_Menu, true, MainScreenClass,
			[this, SubFlow](const EAsyncWidgetLayerState State, const USMMenuLayout* Screen) {
				switch (State)
				{
					case EAsyncWidgetLayerState::AfterPush:
						SubFlow->ContinueFlow();
						break;
					case EAsyncWidgetLayerState::Canceled:
						break;
					default:
						break;
				}
			});
	}
}

#undef LOCTABLE_ID
