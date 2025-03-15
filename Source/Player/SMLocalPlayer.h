// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "UObject/Object.h"
#include "SMLocalPlayer.generated.h"

class UInputMappingContext;
struct FSwapAudioOutputResult;
class USMSettingsShared;
class USMSettingsLocal;

UCLASS()
class STEREOMIX_API USMLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:
	USMLocalPlayer();

	virtual void PostInitProperties() override;
	virtual void SwitchController(class APlayerController* PC) override;
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	virtual void InitOnlineSession() override;

	/**
	 * 플레이어의 로컬 설정을 가져옵니다. 최초 한번 Config 파일에서 읽어오며 항상 유효합니다.
	 * @return 로컬 설정
	 */
	UFUNCTION()
	USMSettingsLocal* GetLocalSettings() const;

	/**
	 * 플레이어의 공유 설정을 가져옵니다. 최초 한번 SaveGame 시스템을 통해 읽어오며 로그인되어 있지 않으면 올바른 설정을 가져오지 못할 수 있습니다.
	 * @return 공유 설정
	 */
	UFUNCTION()
	USMSettingsShared* GetSharedSettings() const;

	/**
	 * 비동기적으로 공유 설정을 로드합니다. 로드가 완료됐거나 존재하지 않아 생성된 이후에는 OnSharedSettingsLoaded를 호출합니다. 
	 */
	void LoadSharedSettingsFromDisk(bool bForceLoad = false);

protected:
	void OnSharedSettingsLoaded(USMSettingsShared* LoadedOrCreatedSettings);

	void OnAudioOutputDeviceChanged(const FString& NewAudioOutputDevice);
	
	UFUNCTION()
	void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

	void OnPlayerControllerChanged(APlayerController* NewController);

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<USMSettingsShared> SharedSettings;

	FUniqueNetIdRepl NetIdForSharedSettings;

	UPROPERTY(Transient)
	mutable TObjectPtr<const UInputMappingContext> InputMappingContext;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
