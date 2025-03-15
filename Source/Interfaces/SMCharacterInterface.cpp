// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterInterface.h"

#include "StereoMixLog.h"


// Add default functionality here for any ISMCharacterInterface functions that are not pure virtual.
void ISMCharacterInterface::ConditionalBroadcastCharacterChanged(TScriptInterface<ISMCharacterInterface> This, ESMCharacterType OldCharacterType, ESMCharacterType NewCharacterType)
{
	if (OldCharacterType != NewCharacterType)
	{
		UObject* ThisObj = This.GetObject();
		This.GetInterface()->GetOnCharacterChangedDelegateChecked().Broadcast(ThisObj, OldCharacterType, NewCharacterType);
	}
}
