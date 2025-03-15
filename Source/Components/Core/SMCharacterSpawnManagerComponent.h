// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "SMCharacterSpawnManagerComponent.generated.h"

class ASMPlayerController;
class ASMPlayerCharacterBase;
class ASMPlayerStartBase;
enum class ESMCharacterType : uint8;

UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName = "StereoMix Character Spawn Manager", meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCharacterSpawnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMCharacterSpawnManagerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	// 플레이어에게 적합한 캐릭터 클래스를 찾습니다.
	TSubclassOf<ASMPlayerCharacterBase> FindCharacterClassForPlayer(const ASMPlayerController* TargetController) const;

private:
	AActor* ChoosePlayerStart(AController* Player) const;
	friend class ASMGameMode;

	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(const AController* Player) const;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "StereoMix|Character")
	TMap<ESMCharacterType, TSoftObjectPtr<USMPlayerCharacterDataAsset>> CharacterSpawnData;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ASMPlayerStartBase>> CachedPlayerStarts;
};
