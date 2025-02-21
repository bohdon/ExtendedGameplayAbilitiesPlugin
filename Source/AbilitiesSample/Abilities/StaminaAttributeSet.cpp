// Copyright Bohdon Sayre, All Rights Reserved.


#include "StaminaAttributeSet.h"

#include "Net/UnrealNetwork.h"

UStaminaAttributeSet::UStaminaAttributeSet()
{
	InitStamina(100.f);
	InitMaxStamina(100.f);
	InitStaminaRegen(25.f);
}

void UStaminaAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetStaminaAttribute())
	{
		// clamp to Max
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		// always keep Max rounded and >= 1
		NewValue = FMath::RoundFromZero(FMath::Max(NewValue, 1.f));
	}
}

void UStaminaAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// apply clamping to base values
	ClampAttribute(Attribute, NewValue);
}

void UStaminaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// also apply clamping to current values (changed by temporary modifiers)
	ClampAttribute(Attribute, NewValue);
}

void UStaminaAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxStaminaAttribute())
	{
		// just clamp down, don't increase proportionally
		if (NewValue < GetStamina())
		{
			SetStamina(NewValue);
		}
	}
}

void UStaminaAttributeSet::OnRep_Stamina(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaAttributeSet, Stamina, OldValue);
}

void UStaminaAttributeSet::OnRep_MaxStamina(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaAttributeSet, MaxStamina, OldValue);
}

void UStaminaAttributeSet::OnRep_StaminaRegen(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaAttributeSet, StaminaRegen, OldValue);
}


void UStaminaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaAttributeSet, StaminaRegen, COND_None, REPNOTIFY_Always);
}
