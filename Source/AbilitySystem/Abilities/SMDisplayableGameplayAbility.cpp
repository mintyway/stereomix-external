// Copyright Studio Surround. All Rights Reserved.


#include "SMDisplayableGameplayAbility.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystem/SMTags.h"
#include "Messages/SMAbilityMessages.h"

USMDisplayableGameplayAbility::USMDisplayableGameplayAbility()
{
	AbilityTags.AddTag(SMTags::Ability::Displayable);
}

void USMDisplayableGameplayAbility::BeginDestroy()
{
	Super::BeginDestroy();

	if (GetWorld())
	{
		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
			{
				FSMGameplayAbilitySlotMessage Message;
				Message.InputID = InputID;
				// Message.AbilitySpecHandle = GetCurrentAbilitySpecHandle();
				MessageSubsystem->BroadcastMessage(SMTags::Ability::Slot_Remove, Message);
			}
		}
	}
}

void USMDisplayableGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (GetWorld())
	{
		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
			{
				FSMGameplayAbilitySlotMessage Message;
				InputID = GetCurrentAbilitySpec()->InputID;
				Message.InputID = InputID;
				Message.AbilitySpecHandle = GetCurrentAbilitySpecHandle();
				MessageSubsystem->BroadcastMessage(SMTags::Ability::Slot_Add, Message);
			}
		}
	}
}
