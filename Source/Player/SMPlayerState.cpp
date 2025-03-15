// Copyright Studio Surround. All Rights Reserved.


#include "SMPlayerState.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "SMGameplayTags.h"
#include "StereoMixLog.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Player/SMPlayerController.h"
#include "Data/SMTeam.h"
#include "FunctionLibraries/SMScoreFunctionLibrary.h"
#include "Games/Round/SMRoundComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Messages/SMAbilityMessages.h"
#include "Messages/SMVerbMessage.h"
#include "Messages/SMVerbMessageHelpers.h"
#include "Net/UnrealNetwork.h"
#include "System/GameplayTagStack.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMPlayerState)

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixPlayerState, Log, All);

ASMPlayerState::ASMPlayerState()
{
	// 어빌리티시스템의 원활한 동기화를 위해 100fps로 설정
	NetUpdateFrequency = 100.0f;

	DefaultTeam = ESMTeam::None;
	DefaultCharacterType = ESMCharacterType::None;

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));
	AbilitySystemComponent = CreateDefaultSubobject<USMAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	CharacterAttributeSet = CreateDefaultSubobject<USMCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

void ASMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyCharacterType, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, StatTags, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PlayerScoreData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TempPlayerScoreData, SharedParams);
}

void ASMPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		if (IsOnlyASpectator())
		{
			TeamComponent->SetTeam(ESMTeam::None);
			SetCharacterType(ESMCharacterType::None);
		}
	}

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void ASMPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ASMPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASMPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);
	// 로컬 플레이어에 한해서 GameplayMessageSubsystem을 이용해서 어빌리티 시스템의 이벤트를 메시지로 전환하여 전파함
	AbilitySystemComponent->OnGameplayEventReceivedEvent.AddUObject(this, &ThisClass::OnClientAbilitySystemGameplayEventReceived);
	AbilitySystemComponent->OnTagUpdatedEvent.AddUObject(this, &ThisClass::OnClientAbilitySystemTagUpdated);
	AbilitySystemComponent->OnGameplayCueAddEvent.AddUObject(this, &ThisClass::OnClientAbilitySystemGameplayCueAdded);
	AbilitySystemComponent->OnGameplayCueRemoveEvent.AddUObject(this, &ThisClass::OnClientAbilitySystemGameplayCueRemoved);
	AbilitySystemComponent->OnGameplayCueExecuteEvent.AddUObject(this, &ThisClass::OnClientAbilitySystemGameplayCueExecuted);
}

void ASMPlayerState::CopyProperties(APlayerState* PlayerState)
{
	if (ASMPlayerState* NewStereoMixPS = Cast<ASMPlayerState>(PlayerState))
	{
		NewStereoMixPS->SetTeam(GetTeam());
		if (UGameplayStatics::HasOption(GetWorld()->GetAuthGameMode()->OptionsString, TEXT("InGame")))
		{
			// Set~ 함수를 호출하면 이벤트가 발생하지만 이 시점에는 플레이어스테이트가 생성된 직후라서 리스너가 없을것으로 예상됨
			NewStereoMixPS->SetCharacterType(GetCharacterType());

			FPlayerScoreData ScoreData = TempPlayerScoreData + PlayerScoreData;
			ScoreData.PlayerState = NewStereoMixPS;
			NewStereoMixPS->SetPlayerScore(ScoreData);
		}
	}
	Super::CopyProperties(PlayerState);
}

FString ASMPlayerState::InitOptions(const FString& Options)
{
	return FString();
}

UAbilitySystemComponent* ASMPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

ASMPlayerController* ASMPlayerState::GetStereoMixPlayerController() const
{
	return Cast<ASMPlayerController>(GetOwner());
}

ESMTeam ASMPlayerState::GetTeam() const
{
	return TeamComponent->GetTeam();
}

void ASMPlayerState::SetTeam(ESMTeam NewTeam)
{
	TeamComponent->SetTeam(NewTeam);
}

USMTeamComponent* ASMPlayerState::GetTeamComponent() const
{
	return TeamComponent;
}

FPlayerScoreData ASMPlayerState::GetPlayerScore() const
{
	return PlayerScoreData;
}

void ASMPlayerState::SetPlayerScore(const FPlayerScoreData& ScoreData)
{
	if (HasAuthority())
	{
		PlayerScoreData = ScoreData;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PlayerScoreData, this);
	}
}

void ASMPlayerState::SetTempPlayerScore(const FPlayerScoreData& ScoreData)
{
	if (HasAuthority())
	{
		TempPlayerScoreData = ScoreData;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TempPlayerScoreData, this);
	}
}

void ASMPlayerState::RequestChangeTeam_Implementation(ESMTeam NewTeam)
{
#if WITH_SERVER_CODE
	GetTeamComponent()->SetTeam(NewTeam);
	GetWorld()->GetAuthGameMode()->RestartPlayer(GetOwningController());
#endif
}

bool ASMPlayerState::RequestChangeTeam_Validate(ESMTeam NewTeam)
{
#if !UE_BUILD_SHIPPING // || UE_WITH_CHEAT_MANAGER
	return true;
#else
	return false;
#endif
}

void ASMPlayerState::RequestChangeCharacter_Implementation(ESMCharacterType NewCharacter)
{
#if WITH_SERVER_CODE
	SetCharacterType(NewCharacter);
	GetWorld()->GetAuthGameMode()->RestartPlayer(GetOwningController());
#endif
}

bool ASMPlayerState::RequestChangeCharacter_Validate(ESMCharacterType NewCharacter)
{
#if !UE_BUILD_SHIPPING // || UE_WITH_CHEAT_MANAGER
	return true;
#else
	return false;
#endif
}

ESMCharacterType ASMPlayerState::GetCharacterType() const
{
	return MyCharacterType;
}

void ASMPlayerState::SetCharacterType(ESMCharacterType CharacterType)
{
	if (HasAuthority())
	{
		const ESMCharacterType OldCharacterType = this->MyCharacterType;
		this->MyCharacterType = CharacterType;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyCharacterType, this);
		ConditionalBroadcastCharacterChanged(this, OldCharacterType, CharacterType);
	}
	else
	{
		UE_LOG(LogStereoMixTeams, Error, TEXT("Cannot set character type for %s on non-authority"), *GetPathName(this));
	}
}

FSMOnCharacterChanged* ASMPlayerState::GetOnCharacterChangedDelegate()
{
	return &OnCharacterChanged;
}

void ASMPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, StatTags, this);
}

void ASMPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, StatTags, this);
}

int32 ASMPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool ASMPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void ASMPlayerState::InitTeamAndCharacterType(const FString& Options)
{
	if (TeamComponent)
	{
		if (GetCharacterType() == ESMCharacterType::None)
		{
			int32 TeamId = UGameplayStatics::GetIntOption(Options, "team", static_cast<int32>(DefaultTeam));
			const ESMTeam Team = static_cast<ESMTeam>(TeamId);
			TeamComponent->SetTeam(Team);

			if (GetTeam() == ESMTeam::None)
			{
				// 퍼포먼스 내다버린 테스트용 임시 코드
				if (USMRoundComponent::Get(GetWorld()))
				{
					int32 Team1Count = 0;
					int32 Team2Count = 0;
					for (APlayerState* PlayerState : GetWorld()->GetGameState()->PlayerArray)
					{
						if (ASMPlayerState* StereoMixPlayerState = Cast<ASMPlayerState>(PlayerState))
						{
							if (StereoMixPlayerState->GetTeam() == ESMTeam::EDM)
							{
								Team1Count++;
							}
							else if (StereoMixPlayerState->GetTeam() == ESMTeam::FutureBass)
							{
								Team2Count++;
							}
						}
					}
					TeamComponent->SetTeam(Team1Count <= Team2Count ? ESMTeam::EDM : ESMTeam::FutureBass);
				}
			}
		}
	}

	if (GetCharacterType() == ESMCharacterType::None)
	{
		int32 CharacterId = UGameplayStatics::GetIntOption(Options, "char", static_cast<int32>(DefaultCharacterType));
		const ESMCharacterType CharacterType = static_cast<ESMCharacterType>(CharacterId);
		SetCharacterType(CharacterType);

		// 퍼포먼스 내다버린 테스트용 임시 코드
		if (GetCharacterType() == ESMCharacterType::None && USMRoundComponent::Get(GetWorld()))
		{
			TSet<ESMCharacterType> CharacterTypes;
			for (APlayerState* PlayerState : GetWorld()->GetGameState()->PlayerArray)
			{
				if (ASMPlayerState* StereoMixPlayerState = Cast<ASMPlayerState>(PlayerState))
				{
					if (StereoMixPlayerState->GetTeam() == GetTeam())
					{
						CharacterTypes.Add(StereoMixPlayerState->GetCharacterType());
					}
				}
			}
			if (!CharacterTypes.Contains(ESMCharacterType::ElectricGuitar))
			{
				SetCharacterType(ESMCharacterType::ElectricGuitar);
			}
			else if (!CharacterTypes.Contains(ESMCharacterType::Piano))
			{
				SetCharacterType(ESMCharacterType::Piano);
			}
			else
			{
				SetCharacterType(ESMCharacterType::Bass);
			}
		}
	}
}

void ASMPlayerState::OnClientAbilitySystemGameplayEventReceived(const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			MessageSubsystem->BroadcastMessage(EventTag, Payload);
		}
	}
}

void ASMPlayerState::OnClientAbilitySystemTagUpdated(const FGameplayTag& UpdatedTag, bool bTagExists)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			MessageSubsystem->BroadcastMessage(UpdatedTag, FSMGameplayTagMessage{ GetPawn(), UpdatedTag, bTagExists });
		}
	}
}

void ASMPlayerState::OnClientAbilitySystemAttributeValueChanged(const FGameplayAttribute& Attribute, const FGameplayAttributeData& OldValue, const FGameplayAttributeData& NewValue)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			MessageSubsystem->BroadcastMessage(StereoMix::Tags::Character_Message_AttributeChanged, FSMGameplayAttributeMessage{ GetPawn(), Attribute, OldValue, NewValue });
		}
	}
}

void ASMPlayerState::OnClientAbilitySystemGameplayCueAdded(const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& CueParameters)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			MessageSubsystem->BroadcastMessage(GameplayCueTag, FSMGameplayCueMessage{ GetPawn(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, CueParameters });
		}
	}
}

void ASMPlayerState::OnClientAbilitySystemGameplayCueRemoved(const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& CueParameters)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			MessageSubsystem->BroadcastMessage(GameplayCueTag, FSMGameplayCueMessage{ GetPawn(), GameplayCueTag, EGameplayCueEvent::Type::Removed, CueParameters });
		}
	}
}

void ASMPlayerState::OnClientAbilitySystemGameplayCueExecuted(const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& CueParameters)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
		{
			MessageSubsystem->BroadcastMessage(GameplayCueTag, FSMGameplayCueMessage{ GetPawn(), GameplayCueTag, EGameplayCueEvent::Type::Executed, CueParameters });
		}
	}
}

void ASMPlayerState::ClientBroadcastMessage_Implementation(const FSMVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

void ASMPlayerState::OnRep_MyCharacterType(ESMCharacterType OldCharacterType)
{
	NET_LOG_CATEGORY(this, LogStereoMixPlayerState, Verbose, TEXT("Character type changed: %s -> %s"), *UEnum::GetValueAsString(OldCharacterType), *UEnum::GetValueAsString(MyCharacterType));
	ConditionalBroadcastCharacterChanged(this, OldCharacterType, MyCharacterType);
}
