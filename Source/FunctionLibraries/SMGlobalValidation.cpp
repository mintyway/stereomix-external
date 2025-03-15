// Copyright Studio Surround. All Rights Reserved.


#include "SMGlobalValidation.h"

bool USMGlobalValidation::ValidateName(const FString& Name)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[가-힣a-zA-Z0-9]{1,10}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, Name);
	return UserNameMatcher.FindNext();
}

bool USMGlobalValidation::ValidateRoomCode(const FString& RoomCode)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[A-Z0-9]{6}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, RoomCode);
	return UserNameMatcher.FindNext();
}
