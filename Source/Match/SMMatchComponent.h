// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "SMMatchComponent.generated.h"


enum class ESMMatchServerType : uint8;
class USMMatchDefinition;

UCLASS(DisplayName = "StereoMix Match Component", meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMMatchComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	USMMatchComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static USMMatchComponent* Get(const UObject* WorldContextObject);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Match")
	ESMMatchServerType GetServerType() const { return ServerType; }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Match")
	const USMMatchDefinition* GetMatchDefinition() const { return MatchDefinition.Get(); }

protected:
	void SetMatchDefinitionId(FName InMatchDefinitionId);
	void SetServerType(ESMMatchServerType InServerType);

	UFUNCTION()
	void OnRep_MatchDefinitionId();

private:
	UPROPERTY(VisibleInstanceOnly, Replicated)
	ESMMatchServerType ServerType;

	UPROPERTY(ReplicatedUsing = OnRep_MatchDefinitionId)
	FName MatchDefinitionId;

	UPROPERTY(VisibleInstanceOnly, Transient)
	TWeakObjectPtr<const USMMatchDefinition> MatchDefinition;
};
