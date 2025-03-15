// Copyright Studio Surround. All Rights Reserved.


#include "SMMatchComponent.h"

#include "SMMatchDefinition.h"
#include "SMMatchLog.h"
#include "SMMatchStatics.h"
#include "SMMatchSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utilities/SMLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMMatchComponent)

USMMatchComponent::USMMatchComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	ServerType = ESMMatchServerType::None;
}

USMMatchComponent* USMMatchComponent::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			if (USMMatchComponent* MatchComponent = GameState->FindComponentByClass<USMMatchComponent>())
			{
				return MatchComponent;
			}
		}
	}
	return nullptr;
}

void USMMatchComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	SharedParams.Condition = COND_InitialOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ServerType, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MatchDefinitionId, SharedParams);
}

void USMMatchComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetWorld()->IsGameWorld() && HasAuthority())
	{
		if (const USMMatchSubsystem* MatchSubsystem = USMMatchSubsystem::Get(this))
		{
			if (const USMMatchDefinition* FoundMatchDefinition = MatchSubsystem->GetMatchDefinition())
			{
				SetServerType(MatchSubsystem->GetServerType());

				FPrimaryAssetId MatchDefinitionAssetId = FoundMatchDefinition->GetPrimaryAssetId();
				SetMatchDefinitionId(MatchDefinitionAssetId.PrimaryAssetName);

				NET_LOG_CATEGORY(GetOwner(), LogStereoMixMatch, Log, TEXT("MatchDefinition set to %s, Server type is %s."),
					*MatchDefinitionAssetId.ToString(),
					*UEnum::GetValueAsString(GetServerType()));
			}
			else
			{
				UE_LOG(LogStereoMixMatch, Error, TEXT("MatchDefinition is not set."));
			}
		}
	}
}

void USMMatchComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USMMatchComponent::SetMatchDefinitionId(FName InMatchDefinitionId)
{
	if (HasAuthority())
	{
		MatchDefinitionId = InMatchDefinitionId;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MatchDefinitionId, this);
	}
}

void USMMatchComponent::SetServerType(ESMMatchServerType InServerType)
{
	if (HasAuthority())
	{
		ServerType = InServerType;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ServerType, this);
	}
}

void USMMatchComponent::OnRep_MatchDefinitionId()
{
	if (USMMatchDefinition* FoundMatchDefinition = USMMatchStatics::FindMatchDefinition(MatchDefinitionId))
	{
		MatchDefinition = FoundMatchDefinition;
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixMatch, Log, TEXT("Received match definition: %s."), *MatchDefinition->GetPrimaryAssetId().ToString());
	}
	else
	{
		NET_LOG_CATEGORY(GetOwner(), LogStereoMixMatch, Error, TEXT("Match definition not found for %s."), *MatchDefinitionId.ToString());
	}
}
