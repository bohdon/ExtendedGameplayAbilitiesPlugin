// Copyright Bohdon Sayre, All Rights Reserved.


#include "StaminaAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaAttributeSet::UStaminaAttributeSet()
{
	InitAttribute(Stamina, 100.f);
	InitAttribute(MaxStamina, 100.f);
	InitAttribute(StaminaRegen, 25.f);

	SetMaxAttribute(GetStaminaAttribute(), GetMaxStaminaAttribute(), false);
	SetAttributeValueRange(GetMaxStaminaAttribute(), 1, FLT_MAX);
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
