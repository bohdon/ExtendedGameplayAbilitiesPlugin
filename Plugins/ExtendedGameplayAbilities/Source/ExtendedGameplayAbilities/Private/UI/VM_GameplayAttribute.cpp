// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_GameplayAttribute.h"

#include "AbilitySystemComponent.h"


void UVM_GameplayAttribute::SetAttribute(FGameplayAttribute NewAttribute)
{
	SetAttributeAndAbilitySystem(NewAttribute, AbilitySystem.Get());
}

void UVM_GameplayAttribute::SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem)
{
	SetAttributeAndAbilitySystem(Attribute, NewAbilitySystem);
}

void UVM_GameplayAttribute::SetAttributeAndAbilitySystem(FGameplayAttribute NewAttribute, UAbilitySystemComponent* NewAbilitySystem)
{
	FGameplayAttribute OldAttribute = Attribute;
	UAbilitySystemComponent* OldAbilitySystem = AbilitySystem.Get();
	if (Attribute == NewAttribute && AbilitySystem.Get() == NewAbilitySystem)
	{
		return;
	}

	if (AbilitySystem.IsValid() && Attribute.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
	}

	Attribute = NewAttribute;
	AbilitySystem = NewAbilitySystem;

	if (AbilitySystem.IsValid() && Attribute.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UVM_GameplayAttribute::OnAttributeValueChanged);
	}

	if (OldAttribute != Attribute)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Attribute);
	}
	if (OldAbilitySystem != AbilitySystem.Get())
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilitySystem);
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetValue);
}

float UVM_GameplayAttribute::GetValue() const
{
	if (AbilitySystem.IsValid())
	{
		bool bFound;
		const float Value = AbilitySystem->GetGameplayAttributeValue(Attribute, bFound);
		if (bFound)
		{
			return Value;
		}
	}
	return 0.f;
}

void UVM_GameplayAttribute::OnAttributeValueChanged(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.Attribute == Attribute)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetValue);
	}
}
