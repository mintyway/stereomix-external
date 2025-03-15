#pragma once

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define ATTRIBUTE_ACCESSORS_REPLICATE(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER_FAST(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER_FAST(PropertyName)

#define GAMEPLAYATTRIBUTE_VALUE_SETTER_FAST(PropertyName) \
FORCEINLINE void Set##PropertyName(float NewVal) \
{ \
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent(); \
	if (ensure(AbilityComp)) \
	{ \
		float OldVal = PropertyName.GetCurrentValue(); \
		AbilityComp->SetNumericAttributeBase(Get##PropertyName##Attribute(), NewVal); \
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PropertyName, this); \
	} \
}

#define GAMEPLAYATTRIBUTE_VALUE_INITTER_FAST(PropertyName) \
FORCEINLINE void Init##PropertyName(float NewVal) \
{ \
	PropertyName.SetBaseValue(NewVal); \
	PropertyName.SetCurrentValue(NewVal); \
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PropertyName, this); \
}
