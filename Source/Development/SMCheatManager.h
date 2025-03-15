// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SMCheatManager.generated.h"

#ifndef USING_CHEAT_MANAGER
#define USING_CHEAT_MANAGER (1 && !UE_BUILD_SHIPPING)
#endif

enum class ESMTeam : uint8;
enum class ESMCharacterType : uint8;

UCLASS(Within = PlayerController, MinimalAPI)
class USMCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ChangeTeam(ESMTeam NewTeam);

	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ChangeCharacter(ESMCharacterType NewCharacter);

	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void HelloStereoMix();

	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void HelloStereoMix2(int32 Number);
};
