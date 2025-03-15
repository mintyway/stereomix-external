// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "CommonUITypes.h"
#include "SMUITypes.generated.h"


UCLASS(Blueprintable, EditInlineNew, CollapseCategories, DisplayName = "StereoMix Input Metadata")
class STEREOMIX_API USMInputMetadata : public UCommonInputMetadata
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonInput")
	bool bShowGuide = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonInput", meta = (InlineEditConditionToggle))
	bool bIsInputTypeSpecificAction = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonInput", meta = (EditCondition = "bIsInputTypeSpecificAction"))
	ECommonInputType SpecificInputType = ECommonInputType::Gamepad;
};

// /**
//  * 
//  */
// UCLASS(BlueprintType, DisplayName = "StereoMix Mapping Context Metadata")
// class STEREOMIX_API USMMappingContextMetadata : public UDataAsset, public ICommonMappingContextMetadataInterface
// {
// 	GENERATED_BODY()
//
// public:
// 	/** Fallback or default metadata CommonUI relies on if no per-action meta is found below */
// 	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "CommonInput")
// 	TObjectPtr<USMInputMetadata> EnhancedInputMetadata;
//
// 	/** Map of action to metadata, allows creation of single metadata asset rather than one per input action type */
// 	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "CommonInput")
// 	TMap<TObjectPtr<UInputAction>, TObjectPtr<const USMInputMetadata>> PerActionEnhancedInputMetadata;
//
// public:
// 	virtual const UCommonInputMetadata* GetCommonInputMetadata(const UInputAction* InInputAction) const override;
// };
