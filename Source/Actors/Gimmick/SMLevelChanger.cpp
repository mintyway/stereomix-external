// Copyright Studio Surround. All Rights Reserved.

#include "SMLevelChanger.h"

#include "Engine/LevelStreaming.h"
#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle/SMObstacleBase.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMLevelChanger::ASMLevelChanger()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	NiagaraComponent->SetAutoActivate(false);

	SubLevels.Add("");
	SubLevels.Add("");
}

#if WITH_EDITOR
void ASMLevelChanger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (const FProperty* PropertyThatChanged = PropertyChangedEvent.Property)
	{
		const FName PropertyName = PropertyThatChanged->GetFName();

		if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ActiveLevelCount))
		{
			if (ActiveLevelCount + 1 > SubLevels.Num())
			{
				for (int32 Index = SubLevels.Num(); Index < ActiveLevelCount + 1; ++Index)
				{
					SubLevels.Add("");
				}
			}
		}

		if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, LevelSwitchInterval) || PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ObstacleSpawnEffectDuration))
		{
			const float ObstacleLoadAndUnloadDuration = ObstacleSpawnEffectDuration + 2.0f;
			if (LevelSwitchInterval <= ObstacleLoadAndUnloadDuration) // 인터벌을 안전하게 스폰과 제거에 소요되는 이펙트 재생시간보다 크게 설정합니다.
			{
				LevelSwitchInterval = ObstacleLoadAndUnloadDuration;
			}
		}
	}
}
#endif

void ASMLevelChanger::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (HasAuthority() && SubLevels.Num() > 0)
	{
		if (StartDelay > 0.0f)
		{
			FTimerHandle TimerHandle;
			TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr(this);
			World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr] {
				if (ThisWeakPtr.Get())
				{
					ThisWeakPtr->StartLevelSwitchInterval();
				}
			}, StartDelay, false);
		}
		else
		{
			StartLevelSwitchInterval();
		}
	}
}

void ASMLevelChanger::MulticastShowActiveEffect_Implementation()
{
	NiagaraComponent->Activate(true);
}

void ASMLevelChanger::StartLevelSwitchInterval()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	SetRandomSubLevel();

	FTimerHandle TimerHandle;
	TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr(this);
	World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr] {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->SetRandomSubLevel();
		}
	}, LevelSwitchInterval, true);
}

void ASMLevelChanger::SetRandomSubLevel()
{
	TArray<FName> AvailableSubLevels;

	if (PreviouslyActiveSubLevels.Num() >= SubLevels.Num())
	{
		PreviouslyActiveSubLevels.Empty();
	}

	for (FName SubLevel : SubLevels)
	{
		if (!IsLevelActive(SubLevel) && !IsLevelPreviouslyActive(SubLevel))
		{
			AvailableSubLevels.Add(SubLevel);
		}
	}

	if (AvailableSubLevels.Num() > 0)
	{
		const FName RandomSubLevel = AvailableSubLevels[FMath::RandRange(0, AvailableSubLevels.Num() - 1)];
		SetLevelVisibility(RandomSubLevel, true);
		MarkLevelAsActive(RandomSubLevel);
	}
}

bool ASMLevelChanger::IsLevelActive(const FName SubLevel) const
{
	return ActiveSubLevels.Contains(SubLevel);
}

bool ASMLevelChanger::IsLevelPreviouslyActive(const FName SubLevel) const
{
	return PreviouslyActiveSubLevels.Contains(SubLevel);
}

void ASMLevelChanger::MarkLevelAsActive(const FName SubLevel)
{
	ActiveSubLevels.AddUnique(SubLevel);
	PreviouslyActiveSubLevels.AddUnique(SubLevel);
}

void ASMLevelChanger::SetLevelVisibility(const FName SubLevel, bool bVisibility)
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || !World)
	{
		return;
	}

	TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr(this);

	if (bVisibility)
	{
		UGameplayStatics::LoadStreamLevel(this, SubLevel, true, false, FLatentActionInfo());
		UE_LOG(LogTemp, Log, TEXT("Level [%s] is loaded."), *SubLevel.ToString());

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, SubLevel] {
			if (ThisWeakPtr.Get())
			{
				ThisWeakPtr->SetLevelVisibility(SubLevel, false);
			}
		}, (LevelSwitchInterval * ActiveLevelCount) + ObstacleSpawnEffectDuration, false);
	}
	else
	{
		float TimeToUnload = 1.0f;

		for (AActor* Actor : TActorRange<ASMObstacleBase>(World))
		{
			ASMObstacleBase* Obstacle = Cast<ASMObstacleBase>(Actor);
			const ULevel* ObstacleLevel = Obstacle ? Obstacle->GetLevel() : nullptr;
			const UObject* ObstacleOuter = ObstacleLevel ? ObstacleLevel->GetOuter() : nullptr;
			if (const FString ObstacleLevelName = GetNameSafe(ObstacleOuter); Obstacle && ObstacleLevelName == SubLevel)
			{
				TimeToUnload = FMath::Max(TimeToUnload, Obstacle->GetDestroyEffectDuration());
				Obstacle->UnloadObstacle();
			}
		}

		MulticastShowActiveEffect();

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, SubLevel] {
			if (ThisWeakPtr.Get())
			{
				UGameplayStatics::UnloadStreamLevel(ThisWeakPtr.Get(), SubLevel, FLatentActionInfo(), true);
				ThisWeakPtr->ActiveSubLevels.Remove(SubLevel);
			}
		}, TimeToUnload, false);
	}
}
