// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_GameplayAttribute.h"

#include "AbilitySystemComponent.h"


void UVM_GameplayAttribute::SetAttribute(FGameplayAttribute NewAttribute)
{
	SetAbilitySystemAndAttribute(AbilitySystem.Get(), NewAttribute);
}

void UVM_GameplayAttribute::SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem)
{
	SetAbilitySystemAndAttribute(NewAbilitySystem, Attribute);
}

void UVM_GameplayAttribute::SetAbilitySystemAndAttribute(UAbilitySystemComponent* NewAbilitySystem, FGameplayAttribute NewAttribute)
{
	if (AbilitySystem.Get() == NewAbilitySystem && Attribute == NewAttribute)
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

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilitySystem);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Attribute);

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
