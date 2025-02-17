// Copyright Bohdon Sayre, All Rights Reserved.


#include "HPRegenAttributeSet.h"

#include "Net/UnrealNetwork.h"


UHPRegenAttributeSet::UHPRegenAttributeSet()
{
	InitHPRegen(0.f);
}

void UHPRegenAttributeSet::OnRep_HPRegen(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHPRegenAttributeSet, HPRegen, OldValue);
}

void UHPRegenAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHPRegenAttributeSet, HPRegen, COND_None, REPNOTIFY_Always);
}
