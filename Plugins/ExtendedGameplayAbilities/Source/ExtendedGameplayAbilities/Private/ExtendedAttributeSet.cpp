// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "ExtendedAbilitySystemStatics.h"


void UExtendedAttributeSet::SetMaxAttribute(const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute, bool bProportional)
{
	FExtendedMaxAttributeRules& Rules = MaxAttributesMap.FindOrAdd(Attribute);
	Rules.MaxAttribute = MaxAttribute;
	Rules.bProportional = bProportional;
}

void UExtendedAttributeSet::SetAttributeValueRange(const FGameplayAttribute& Attribute, float Min, float Max)
{
	MinMaxValuesMap.Emplace(Attribute, FFloatRange(Min, Max));
}

void UExtendedAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UExtendedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UExtendedAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	AdjustOrClampForMaxAttribute(Attribute, OldValue, NewValue);
}

void UExtendedAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (const FExtendedMaxAttributeRules* MaxAttributeRules = MaxAttributesMap.Find(Attribute))
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxAttributeRules->MaxAttribute.GetNumericValue(this));
	}

	if (const FFloatRange* MinMaxValues = MinMaxValuesMap.Find(Attribute))
	{
		const float Min = MinMaxValues->HasLowerBound() ? MinMaxValues->GetLowerBoundValue() : FLT_MIN;
		const float Max = MinMaxValues->HasUpperBound() ? MinMaxValues->GetUpperBoundValue() : FLT_MAX;
		NewValue = FMath::Clamp(NewValue, Min, Max);
	}
}

void UExtendedAttributeSet::AdjustOrClampForMaxAttribute(const FGameplayAttribute& MaxAttribute, float OldMaxValue, float NewMaxValue)
{
	for (const TTuple<FGameplayAttribute, FExtendedMaxAttributeRules>& Elem : MaxAttributesMap)
	{
		if (Elem.Value.MaxAttribute == MaxAttribute)
		{
			const FGameplayAttribute& Attribute = Elem.Key;
			if (Elem.Value.bProportional)
			{
				// keep proportional and clamp
				UExtendedAbilitySystemStatics::AdjustProportionalAttribute(this, Attribute, OldMaxValue, NewMaxValue, false, true);
			}
			else
			{
				// just clamp
				if (UAbilitySystemComponent* AbilitySystem = GetOwningAbilitySystemComponent())
				{
					const float CurrentValue = UExtendedAbilitySystemStatics::GetNumericAttributeBase(this, Attribute);
					const float NewValue = FMath::Min(CurrentValue, NewMaxValue);
					AbilitySystem->SetNumericAttributeBase(Attribute, NewValue);
				}
			}
		}
	}
}

void UExtendedAttributeSet::InitAttribute(FGameplayAttributeData& AttributeData, float Value)
{
	AttributeData.SetBaseValue(Value);
	AttributeData.SetCurrentValue(Value);
}
