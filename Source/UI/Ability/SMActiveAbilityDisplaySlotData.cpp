// Copyright Studio Surround. All Rights Reserved.


#include "SMActiveAbilityDisplaySlotData.h"

const FSMActiveAbilityDisplayEntryData* USMActiveAbilityDisplaySlotData::FindEntryDataByInputID(int32 InputID) const
{
	return Entries.FindByPredicate([InputID](const FSMActiveAbilityDisplayEntryData& Entry)
	{
		return Entry.GetInputID() == InputID;
	});
}
