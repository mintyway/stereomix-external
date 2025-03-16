﻿// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValue.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "SMSettingKeyboardInput.generated.h"


UCLASS(MinimalAPI)
class USMSettingKeyboardInput : public UGameSettingValue
{
	GENERATED_BODY()

public:
	USMSettingKeyboardInput();

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	void InitializeInputData(const UEnhancedPlayerMappableKeyProfile* KeyProfile, const FKeyMappingRow& MappingData, const FPlayerMappableKeyQueryOptions& InQueryOptions);
	FText GetKeyTextFromSlot(const EPlayerMappableKeySlot InSlot) const;

	bool ChangeBinding(int32 InKeyBindSlot, FKey NewKey);
	void GetAllMappedActionsFromKey(int32 InKeyBindSlot, FKey Key, TArray<FName>& OutActionNames) const;

	/** Returns true if mappings on this setting have been customized */
	bool IsMappingCustomized() const;

	FText GetSettingDisplayName() const;
	FText GetSettingDisplayCategory() const;

	const FKeyMappingRow* FindKeyMappingRow() const;
	UEnhancedPlayerMappableKeyProfile* FindMappableKeyProfile() const;
	UEnhancedInputUserSettings* GetUserSettings() const;

protected:
	virtual void OnInitialized() override;

	/** The name of this action's mappings */
	FName ActionMappingName;

	/** The query options to filter down keys on this setting for */
	FPlayerMappableKeyQueryOptions QueryOptions;

	/** The profile identifier that this key setting is from */
	FGameplayTag ProfileIdentifier;

	/** Store the initial key mappings that are set on this for each slot */
	TMap<EPlayerMappableKeySlot, FKey> InitialKeyMappings;
};
