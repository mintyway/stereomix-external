// Copyright Studio Surround. All Rights Reserved.


#include "SMPlatformSettings.h"

#include "OnlineSubsystemUtils.h"

IOnlineSubsystem* FSMPlatformData::GetOnlineSubsystem() const
{
	return Online::GetSubsystem(nullptr, PlatformName);
}
