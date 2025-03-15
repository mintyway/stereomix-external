// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMCharacterType.h"
#include "UObject/Interface.h"
#include "SMCharacterInterface.generated.h"

template<typename InterfaceType>
class TScriptInterface;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSMOnCharacterChanged, UObject* ObjectChangingCharacter, ESMCharacterType OldCharacterType, ESMCharacterType NewCharacterType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSMOnCharacterChangedDynamic, UObject*, ObjectChangingCharacter, ESMCharacterType, OldCharacterType, ESMCharacterType, NewCharacterType);

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class USMCharacterInterface : public UInterface
{
	GENERATED_BODY()
};


class STEREOMIX_API ISMCharacterInterface
{
	GENERATED_BODY()

public:
	virtual void SetCharacterType(ESMCharacterType InCharacterType)
	{
	}

	virtual ESMCharacterType GetCharacterType() const
	{
		return ESMCharacterType::None;
	}
	
	virtual FSMOnCharacterChanged* GetOnCharacterChangedDelegate() { return nullptr; }

	static void ConditionalBroadcastCharacterChanged(TScriptInterface<ISMCharacterInterface> This, ESMCharacterType OldCharacterType, ESMCharacterType NewCharacterType);

	FSMOnCharacterChanged& GetOnCharacterChangedDelegateChecked()
	{
		FSMOnCharacterChanged* Result = GetOnCharacterChangedDelegate();
		check(Result);
		return *Result;
	}
};
