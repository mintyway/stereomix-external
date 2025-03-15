// Copyright Studio Surround. All Rights Reserved.


#include "SMAssetManagerFunctionLibrary.h"


FPrimaryAssetId USMAssetFunctionLibrary::GetCharacterDataAssetID(ESMCharacterType CharacterType)
{
	FName AssetName;
	switch (CharacterType)
	{
		case ESMCharacterType::ElectricGuitar:
		{
			AssetName = TEXT("DA_ElectricGuitarCharacter");
			break;
		}
		case ESMCharacterType::Piano:
		{
			AssetName = TEXT("DA_PianoCharacter");
			break;
		}
		case ESMCharacterType::Bass:
		{
			AssetName = TEXT("DA_BassCharacter");
			break;
		}
		default: ;
	}

	return FPrimaryAssetId(TEXT("Character"), AssetName);
}
