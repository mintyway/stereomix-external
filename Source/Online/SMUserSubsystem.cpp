// Copyright Studio Surround. All Rights Reserved.


#include "SMUserSubsystem.h"

#include "IEOSSDKManager.h"
#include "IOnlineSubsystemEOS.h"
#include "SMOnlineStatics.h"
#include "eos_connect_types.h"
#include "eos_sdk.h"

DEFINE_LOG_CATEGORY_STATIC(LogStereoMixUser, Log, All);

FString USMUserInfo::GetToken() const
{
#if WITH_EOS_SDK
	TArray<IEOSPlatformHandlePtr> ActiveEOSPlatforms = IEOSSDKManager::Get()->GetActivePlatforms();
	if (ActiveEOSPlatforms.Num() == 0)
	{
		UE_LOG(LogStereoMixUser, Error, TEXT("GetToken: No active EOS platform found for user %d"), LocalPlayerIndex);
		return FString();
	}

	IEOSPlatformHandlePtr PlatformHandlePtr = ActiveEOSPlatforms[0];
	if (PlatformHandlePtr.IsValid())
	{
		EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(*PlatformHandlePtr);

		FString ProductUserIdStr = USMOnlineStatics::GetProductUserId(GetNetId());
		EOS_Connect_CopyIdTokenOptions CopyIdTokenOptions = {
			EOS_CONNECT_COPYIDTOKEN_API_LATEST,
			EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*ProductUserIdStr))
		};

		EOS_Connect_IdToken* IdToken;
		EOS_EResult Result = EOS_Connect_CopyIdToken(ConnectHandle, &CopyIdTokenOptions, &IdToken);
		if (Result == EOS_EResult::EOS_Success)
		{
			FString Token(IdToken->JsonWebToken);
			EOS_Connect_IdToken_Release(IdToken);
			return Token;
		}
	}
	UE_LOG(LogStereoMixUser, Error, TEXT("GetToken: Failed to copy id token for user %d"), LocalPlayerIndex);
	return FString();
#else
	UE_LOG(LogStereoMixUser, Error, TEXT("GetToken: Not implemented get EOS ID token for user %d"), LocalPlayerIndex);
	return FString();
#endif
}

UCommonUserInfo* USMUserSubsystem::CreateLocalUserInfo(int32 LocalPlayerIndex)
{
	UCommonUserInfo* NewUser = nullptr;
	if (ensure(!LocalUserInfos.Contains(LocalPlayerIndex)))
	{
		NewUser = NewObject<USMUserInfo>(this);
		NewUser->LocalPlayerIndex = LocalPlayerIndex;
		NewUser->InitializationState = ECommonUserInitializationState::Unknown;

		// Always create game and default cache
		NewUser->CachedDataMap.Add(ECommonUserOnlineContext::Game, UCommonUserInfo::FCachedData());
		NewUser->CachedDataMap.Add(ECommonUserOnlineContext::Default, UCommonUserInfo::FCachedData());

		// Add platform if needed
		if (HasSeparatePlatformContext())
		{
			NewUser->CachedDataMap.Add(ECommonUserOnlineContext::Platform, UCommonUserInfo::FCachedData());
		}

		LocalUserInfos.Add(LocalPlayerIndex, NewUser);
	}
	return NewUser;
}

const USMUserInfo* USMUserSubsystem::GetStereoMixUserInfoForLocalPlayerIndex(int32 LocalPlayerIndex) const
{
	return Cast<USMUserInfo>(GetUserInfoForLocalPlayerIndex(LocalPlayerIndex));
}
