// Copyright Studio Surround. All Rights Reserved.


#include "SMOnlineTypes.h"

//
// bool FSMOnlineAttributes::GetAttributeAsString(FName Key, FString& Value) const
// {
// 	if (const FVariantData* AttributeValue = Attributes.Find(Key))
// 	{
// 		if (AttributeValue->GetType() == EOnlineKeyValuePairDataType::String)
// 		{
// 			AttributeValue->GetValue(Value);
// 			return true;
// 		}
// 	}
// 	return false;
// }
//
// bool FSMOnlineAttributes::GetAttributeAsInt(FName Key, int32& Value) const
// {
// 	if (const FVariantData* AttributeValue = Attributes.Find(Key))
// 	{
// 		if (AttributeValue->GetType() == EOnlineKeyValuePairDataType::Int32)
// 		{
// 			AttributeValue->GetValue(Value);
// 			return true;
// 		}
// 	}
// 	return false;
// }
//
// bool FSMOnlineAttributes::GetAttributeAsFloat(FName Key, float& Value) const
// {
// 	if (const FVariantData* AttributeValue = Attributes.Find(Key))
// 	{
// 		if (AttributeValue->GetType() == EOnlineKeyValuePairDataType::Float)
// 		{
// 			AttributeValue->GetValue(Value);
// 			return true;
// 		}
// 	}
// 	return false;
// }
//
// bool FSMOnlineAttributes::GetAttributeAsBool(FName Key, bool& Value) const
// {
// 	if (const FVariantData* AttributeValue = Attributes.Find(Key))
// 	{
// 		if (AttributeValue->GetType() == EOnlineKeyValuePairDataType::Bool)
// 		{
// 			AttributeValue->GetValue(Value);
// 			return true;
// 		}
// 	}
// 	return false;
// }
//
// FString FSMOnlineAttributes::GetDebugString() const
// {
// 	TStringBuilder<1024> DebugStringBuilder;
// 	for (const auto& Attribute : Attributes)
// 	{
// 		FString KeyString = Attribute.Key.ToString();
// 		FString StringValue;
// 		int32 IntValue;
// 		switch (Attribute.Value.GetType())
// 		{
// 			case EOnlineKeyValuePairDataType::String:
// 				Attribute.Value.GetValue(StringValue);
// 				DebugStringBuilder.Appendf(TEXT("%s:%s|"), *KeyString, *StringValue);
// 				break;
//
// 			case EOnlineKeyValuePairDataType::Int32:
// 				Attribute.Value.GetValue(IntValue);
// 				DebugStringBuilder.Appendf(TEXT("%s:%d|"), *KeyString, IntValue);
// 				break;
//
// 			default:
// 				DebugStringBuilder.Appendf(TEXT("%s:%s|"), *KeyString, TEXT("<UnsupportedType>"));
// 				break;
// 		}
// 	}
// 	if (Attributes.Num() > 0)
// 	{
// 		DebugStringBuilder.RemoveSuffix(1);
// 	}
// 	return DebugStringBuilder.ToString();
// }
