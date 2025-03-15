// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserSubsystem.h"
#include "SMUserSubsystem.generated.h"

UCLASS()
class USMUserInfo : public UCommonUserInfo
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	FString GetToken() const;
};

UCLASS(DisplayName = "StereoMix User Interface")
class STEREOMIX_API USMUserSubsystem : public UCommonUserSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	const USMUserInfo* GetStereoMixUserInfoForLocalPlayerIndex(int32 LocalPlayerIndex) const;

protected:
	virtual UCommonUserInfo* CreateLocalUserInfo(int32 LocalPlayerIndex) override;
};
