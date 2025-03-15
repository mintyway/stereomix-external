// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamSelectTriggerBox.h"

#include "GameFramework/GameModeBase.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Player/SMPlayerController.h"
#include "Player/SMPlayerState.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

class ISMTeamInterface;

ASMTeamSelectTriggerBox::ASMTeamSelectTriggerBox()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	BaseMesh->SetRelativeLocation(FVector(75.0, 75.0, 1.0));

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(SMCollisionObjectChannel::Player, ECR_Overlap);
	TriggerBox->SetRelativeLocation(FVector(75.0, 75.0, 100.0));
	TriggerBox->InitBoxExtent(FVector(75.0, 75.0, 100.0));

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));
}

void ASMTeamSelectTriggerBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!HasAuthority())
	{
		TriggerBox->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	}
}

void ASMTeamSelectTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	RespawnCharacter(OtherActor);
}

void ASMTeamSelectTriggerBox::RespawnCharacter(AActor* TargetActor)
{
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	ASMPlayerController* TargetController = TargetCharacter->GetController<ASMPlayerController>();
	if (!ensureAlways(TargetController))
	{
		return;
	}

	ASMPlayerState* OtherPlayerState = TargetController->GetPlayerState<ASMPlayerState>();
	if (!ensureAlways(OtherPlayerState))
	{
		return;
	}

	OtherPlayerState->GetTeamComponent()->SetTeam(TeamComponent->GetTeam());
	OtherPlayerState->SetCharacterType(CharacterType);

	// 기존의 캐릭터를 제거하고 게임모드를 통해 새로운 캐릭터를 스폰합니다.
	TargetActor->Destroy();
	GetWorld()->GetAuthGameMode()->RestartPlayer(OtherPlayerState->GetOwningController());

	const FString TeamName = UEnum::GetValueAsString(TEXT("StereoMix.ESMTeam"), TeamComponent->GetTeam());
	NET_LOG(this, Log, TEXT("%s의 팀이 %s, 캐릭터는 %s로 변경되었습니다."), *TargetCharacter->GetName(), *TeamName, *UEnum::GetValueAsString(CharacterType));
}
