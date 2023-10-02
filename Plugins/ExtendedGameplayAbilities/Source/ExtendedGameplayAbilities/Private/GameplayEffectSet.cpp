// Copyright Bohdon Sayre, All Rights Reserved.

#include "GameplayEffectSet.h"


// FGameplayEffectSet
// ------------------

bool FGameplayEffectSet::IsEmpty() const
{
	return Effects.IsEmpty();
}


// FGameplayEffectSpecSet
// ----------------------

bool FGameplayEffectSpecSet::IsEmpty() const
{
	return EffectSpecs.IsEmpty();
}
