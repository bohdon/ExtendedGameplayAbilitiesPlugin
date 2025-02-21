// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_GameplayAttribute.h"

#include "AbilitySystemComponent.h"


void UVM_GameplayAttribute::SetAttribute(FGameplayAttribute NewAttribute)
{
	SetAbilitySystemAndAttribute(AbilitySystem.Get(), NewAttribute);
}

void UVM_GameplayAttribute::SetAbilitySystemAndAttribute(UAbilitySystemComponent* NewAbilitySystem, FGameplayAttribute NewAttribute)
{
	if (AbilitySystem.Get() != NewAbilitySystem || Attribute != NewAttribute)
	{
		PreSystemChange();
		Attribute = NewAttribute;
		AbilitySystem = NewAbilitySystem;
		PostSystemChange();
	}
}

void UVM_GameplayAttribute::PreSystemChange()
{
	if (AbilitySystem.IsValid() && Attribute.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
	}

	Super::PreSystemChange();
}

void UVM_GameplayAttribute::PostSystemChange()
{
	if (AbilitySystem.IsValid() && Attribute.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UVM_GameplayAttribute::OnAttributeValueChanged);
	}

	Super::PostSystemChange();

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
