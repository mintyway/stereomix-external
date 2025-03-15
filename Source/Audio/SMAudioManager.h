// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMAudioSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMAudioManager.generated.h"

struct FFMODEventInstance;
class UFMODEvent;


UCLASS()
class STEREOMIX_API USMAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USMAudioManager* Get(const UObject* ContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	void OnAppActivationStateChanged(bool bActivated) const;

private:
	FDelegateHandle OnApplicationActivationStateChangedHandle;

	UPROPERTY(Transient)
	TObjectPtr<UFMODBus> MasterBus;
};
