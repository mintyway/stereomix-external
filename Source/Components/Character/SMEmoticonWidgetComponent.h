// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Interfaces/SMInputBindableInterface.h"
#include "SMEmoticonWidgetComponent.generated.h"


class USMEmoticonWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMEmoticonWidgetComponent : public UWidgetComponent, public ISMInputBindableInterface
{
	GENERATED_BODY()

public:
	USMEmoticonWidgetComponent();

	virtual void InitializeComponent() override;

	virtual void SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent) override;

	void PlayEmoticon1();

	void PlayEmoticon2();

	void PlayEmoticon3();

protected:
	virtual void BeginPlay() override;

	void OnEmoticon1Input();

	void OnEmoticon2Input();

	void OnEmoticon3Input();

	static TArray<USMEmoticonWidgetComponent*> GetEmoticonComponentExcludingCaster(const UObject* WorldContextObject, const AActor* Caster);

	UPROPERTY()
	TObjectPtr<const USMCommonCharacterDataAsset> OwnerCommonDataAsset;

	UPROPERTY()
	TObjectPtr<USMEmoticonWidget> EmoticonWidget;

private:
	UFUNCTION(Server, Unreliable)
	void ServerSendEmoticon1Input(AActor* Caster);

	UFUNCTION(Server, Unreliable)
	void ServerSendEmoticon2Input(AActor* Caster);

	UFUNCTION(Server, Unreliable)
	void ServerSendEmoticon3Input(AActor* Caster);

	UFUNCTION(Client, Unreliable)
	void ClientPlayEmoticon1(USMEmoticonWidgetComponent* CasterComponent);

	UFUNCTION(Client, Unreliable)
	void ClientPlayEmoticon2(USMEmoticonWidgetComponent* CasterComponent);

	UFUNCTION(Client, Unreliable)
	void ClientPlayEmoticon3(USMEmoticonWidgetComponent* CasterComponent);
};
