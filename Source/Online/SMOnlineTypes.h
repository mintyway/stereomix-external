// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineKeyValuePair.h"
#include "StereoMixLog.h"
#include "UObject/Object.h"
#include "SMOnlineTypes.generated.h"

// USTRUCT(BlueprintType)
// struct STEREOMIX_API FSMOnlineAttributes
// {
// 	GENERATED_BODY()
//
// 	virtual ~FSMOnlineAttributes() = default;
// 	
// 	virtual FString GetDebugString() const;
// 	
// 	bool GetAttributeAsString(FName Key, FString& Value) const;
// 	bool GetAttributeAsInt(FName Key, int32& Value) const;
// 	bool GetAttributeAsFloat(FName Key, float& Value) const;
// 	bool GetAttributeAsBool(FName Key, bool& Value) const;
//
// 	TMap<FName, FVariantData> Attributes;
// };

UENUM(BlueprintType)
enum class ESMOnlineAttributeType : uint8
{
	String,
	Bool,
	Integer,
	Double
};

USTRUCT(BlueprintType)
struct FSMOnlineAttribute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix|Online")
	ESMOnlineAttributeType AttributeType = ESMOnlineAttributeType::String;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix|Online")
	FString StringValue = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix|Online")
	bool BoolValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix|Online")
	int64 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix|Online")
	double DoubleValue = 0.0;

	FSMOnlineAttribute()
	{
		AttributeType = ESMOnlineAttributeType::String;
		StringValue = "";
		BoolValue = false;
		IntValue = 0;
		DoubleValue = 0.0;
	}

	FVariantData GetVariantData() const
	{
		FVariantData VariantData;
		switch (AttributeType)
		{
			case ESMOnlineAttributeType::String:
				VariantData.SetValue(StringValue);
				break;
			case ESMOnlineAttributeType::Bool:
				VariantData.SetValue(BoolValue);
				break;
			case ESMOnlineAttributeType::Integer:
				VariantData.SetValue(IntValue);
				break;
			case ESMOnlineAttributeType::Double:
				VariantData.SetValue(DoubleValue);
				break;
			default:
				VariantData.SetValue(StringValue);
				break;
		}
		return VariantData;
	}

	FSMOnlineAttribute(const FVariantData& VariantData)
	{
		switch (VariantData.GetType())
		{
			case EOnlineKeyValuePairDataType::String:
				AttributeType = ESMOnlineAttributeType::String;
				VariantData.GetValue(StringValue);
				break;
			case EOnlineKeyValuePairDataType::Bool:
				AttributeType = ESMOnlineAttributeType::Bool;
				VariantData.GetValue(BoolValue);
				break;
			case EOnlineKeyValuePairDataType::Int64:
				AttributeType = ESMOnlineAttributeType::Integer;
				VariantData.GetValue(IntValue);
				break;
			case EOnlineKeyValuePairDataType::Double:
				AttributeType = ESMOnlineAttributeType::Double;
				VariantData.GetValue(DoubleValue);
				break;
			default:
				UE_LOG(LogStereoMixOnline, Warning, TEXT("Unsupported attribute type: %d"), VariantData.GetType());
				break;
		}
	}
};
