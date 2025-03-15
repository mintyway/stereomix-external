// Copyright Studio Surround. All Rights Reserved.


#include "SMEmoticonWidgetComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Player/SMPlayerController.h"
#include "UI/Emoticon/SMEmoticonWidget.h"
#include "Utilities/SMLog.h"


USMEmoticonWidgetComponent::USMEmoticonWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void USMEmoticonWidgetComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const ASMPlayerCharacterBase* OwnerCharacter = GetOwner<ASMPlayerCharacterBase>();
	const USMPlayerCharacterDataAsset* OwnerDataAsset = OwnerCharacter ? OwnerCharacter->GetDataAsset() : nullptr;
	OwnerCommonDataAsset = OwnerDataAsset ? OwnerDataAsset->GetCommonDataAsset() : nullptr;

	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
}

void USMEmoticonWidgetComponent::SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (EnhancedInputComponent && OwnerCommonDataAsset)
	{
		EnhancedInputComponent->BindAction(OwnerCommonDataAsset->Emoticon1Action, ETriggerEvent::Started, this, &ThisClass::OnEmoticon1Input);
		EnhancedInputComponent->BindAction(OwnerCommonDataAsset->Emoticon2Action, ETriggerEvent::Started, this, &ThisClass::OnEmoticon2Input);
		EnhancedInputComponent->BindAction(OwnerCommonDataAsset->Emoticon3Action, ETriggerEvent::Started, this, &ThisClass::OnEmoticon3Input);
	}
}

void USMEmoticonWidgetComponent::PlayEmoticon1()
{
	if (EmoticonWidget)
	{
		EmoticonWidget->PlayHiEmoticon();
	}
}

void USMEmoticonWidgetComponent::PlayEmoticon2()
{
	if (EmoticonWidget)
	{
		EmoticonWidget->PlayGoodEmoticon();
	}
}

void USMEmoticonWidgetComponent::PlayEmoticon3()
{
	if (EmoticonWidget)
	{
		EmoticonWidget->PlaySadEmoticon();
	}
}

void USMEmoticonWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerCommonDataAsset)
	{
		SetWidgetClass(OwnerCommonDataAsset->EmoticonWidgetClass);
		EmoticonWidget = Cast<USMEmoticonWidget>(GetWidget());
	}
}

void USMEmoticonWidgetComponent::OnEmoticon1Input()
{
	PlayEmoticon1();

	if (GetOwnerRole() != ROLE_Authority)
	{
		ServerSendEmoticon1Input(GetOwner());
	}
}

void USMEmoticonWidgetComponent::OnEmoticon2Input()
{
	PlayEmoticon2();

	if (GetOwnerRole() != ROLE_Authority)
	{
		ServerSendEmoticon2Input(GetOwner());
	}
}

void USMEmoticonWidgetComponent::OnEmoticon3Input()
{
	PlayEmoticon3();

	if (GetOwnerRole() != ROLE_Authority)
	{
		ServerSendEmoticon3Input(GetOwner());
	}
}

TArray<USMEmoticonWidgetComponent*> USMEmoticonWidgetComponent::GetEmoticonComponentExcludingCaster(const UObject* WorldContextObject, const AActor* Caster)
{
	TArray<USMEmoticonWidgetComponent*> Result;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World || !Caster)
	{
		return Result;
	}

	for (const ASMPlayerController* PlayerController : TActorRange<ASMPlayerController>(World))
	{
		if (!PlayerController || PlayerController->GetPawn() == Caster)
		{
			continue;
		}

		const APawn* ExcludingCasterPawn = PlayerController->GetPawn();
		if (USMEmoticonWidgetComponent* ExcludingCasterEmoticonComponent = ExcludingCasterPawn ? ExcludingCasterPawn->GetComponentByClass<ThisClass>() : nullptr)
		{
			Result.Add(ExcludingCasterEmoticonComponent);
		}
	}

	return Result;
}

void USMEmoticonWidgetComponent::ServerSendEmoticon1Input_Implementation(AActor* Caster)
{
	for (USMEmoticonWidgetComponent* ExcludingCasterEmoticonComponent : GetEmoticonComponentExcludingCaster(this, Caster))
	{
		if (ExcludingCasterEmoticonComponent)
		{
			ExcludingCasterEmoticonComponent->ClientPlayEmoticon1(this);
		}
	}
}

void USMEmoticonWidgetComponent::ServerSendEmoticon2Input_Implementation(AActor* Caster)
{
	for (USMEmoticonWidgetComponent* ExcludingCasterEmoticonComponent : GetEmoticonComponentExcludingCaster(this, Caster))
	{
		if (ExcludingCasterEmoticonComponent)
		{
			ExcludingCasterEmoticonComponent->ClientPlayEmoticon2(this);
		}
	}
}

void USMEmoticonWidgetComponent::ServerSendEmoticon3Input_Implementation(AActor* Caster)
{
	for (USMEmoticonWidgetComponent* ExcludingCasterEmoticonComponent : GetEmoticonComponentExcludingCaster(this, Caster))
	{
		if (ExcludingCasterEmoticonComponent)
		{
			ExcludingCasterEmoticonComponent->ClientPlayEmoticon3(this);
		}
	}
}

void USMEmoticonWidgetComponent::ClientPlayEmoticon1_Implementation(USMEmoticonWidgetComponent* CasterComponent)
{
	if (CasterComponent)
	{
		CasterComponent->PlayEmoticon1();
	}
}

void USMEmoticonWidgetComponent::ClientPlayEmoticon2_Implementation(USMEmoticonWidgetComponent* CasterComponent)
{
	if (CasterComponent)
	{
		CasterComponent->PlayEmoticon2();
	}
}

void USMEmoticonWidgetComponent::ClientPlayEmoticon3_Implementation(USMEmoticonWidgetComponent* CasterComponent)
{
	if (CasterComponent)
	{
		CasterComponent->PlayEmoticon3();
	}
}
