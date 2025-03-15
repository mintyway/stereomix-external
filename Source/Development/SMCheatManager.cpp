// Copyright Studio Surround. All Rights Reserved.


#include "SMCheatManager.h"

#include "Player/SMPlayerState.h"

void USMCheatManager::ChangeTeam(ESMTeam NewTeam)
{
	if (APlayerController* PC = GetPlayerController())
	{
		if (ASMPlayerState* StereoMixPS = PC->GetPlayerState<ASMPlayerState>())
		{
			StereoMixPS->RequestChangeTeam(NewTeam);
		}
	}
}

void USMCheatManager::ChangeCharacter(ESMCharacterType NewCharacter)
{
	if (APlayerController* PC = GetPlayerController())
	{
		if (ASMPlayerState* StereoMixPS = PC->GetPlayerState<ASMPlayerState>())
		{
			StereoMixPS->RequestChangeCharacter(NewCharacter);
		}
	}
}

void USMCheatManager::HelloStereoMix()
{
	UE_LOG(LogTemp, Warning, TEXT("Hello StereoMix!"));
}

void USMCheatManager::HelloStereoMix2(int32 Number)
{
	UE_LOG(LogTemp, Warning, TEXT("Hello StereoMix! Number: %d"), Number);
}
