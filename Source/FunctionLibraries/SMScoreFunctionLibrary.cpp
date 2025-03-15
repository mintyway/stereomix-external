// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreFunctionLibrary.h"

#include "GameFramework/GameStateBase.h"
#include "SMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/Core/SMScoreManagerComponent.h"


USMScoreManagerComponent* USMScoreFunctionLibrary::GetScoreManager(const UWorld* World)
{
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	return GameState ? GameState->GetComponentByClass<USMScoreManagerComponent>() : nullptr;
}

FPlayerScoreData USMScoreFunctionLibrary::GetPlayerScoreData(const UWorld* World, const ASMPlayerState* PlayerState)
{
	if (const USMScoreManagerComponent* ScoreManagerComponent = GetScoreManager(World))
	{
		return ScoreManagerComponent->GetPlayerScoreData(PlayerState);
	}

	return FPlayerScoreData();
}

int32 USMScoreFunctionLibrary::GetTeamScore(const UWorld* World, ESMTeam Team)
{
	int32 TotalScore = 0;
	if (USMScoreManagerComponent* ScoreManagerComponent = GetScoreManager(World))
	{
		for (const auto& ScoreData : ScoreManagerComponent->GetTeamScoreData(Team))
		{
			TotalScore += ScoreData.TotalScore();
		}
	}

	return TotalScore;
}

TArray<FPlayerScoreData> USMScoreFunctionLibrary::GetTeamScoreData(const UWorld* World, ESMTeam Team)
{
	if (USMScoreManagerComponent* ScoreManagerComponent = GetScoreManager(World))
	{
		return ScoreManagerComponent->GetTeamScoreData(Team);
	}

	return TArray<FPlayerScoreData>();
}

void USMScoreFunctionLibrary::RecordDamage(AActor* Self, const AActor* Attacker, float InDamageAmount)
{
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(Self);
	if (!SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Invincible))
	{
		if (USMScoreManagerComponent* ScoreManagerComponent = GetScoreManager(Self->GetWorld()))
		{
			ScoreManagerComponent->AddTotalDamageReceived(Self, InDamageAmount);
			ScoreManagerComponent->AddTotalDamageDealt(Attacker, InDamageAmount);
		}
	}
}

void USMScoreFunctionLibrary::RecordNoiseBreakUsage(const AActor* Self)
{
	if (USMScoreManagerComponent* ScoreManagerComponent = Self ? GetScoreManager(Self->GetWorld()) : nullptr)
	{
		ScoreManagerComponent->AddTotalNoiseBreakUsage(Self);
	}
}

void USMScoreFunctionLibrary::RecordKillDeathCount(const AActor* LastAttacker, const AActor* Self)
{
	if (USMScoreManagerComponent* ScoreManagerComponent = Self ? GetScoreManager(Self->GetWorld()) : nullptr)
	{
		ScoreManagerComponent->AddTotalKillCount(LastAttacker);
		ScoreManagerComponent->AddTotalDeathCount(Self);
	}
}
