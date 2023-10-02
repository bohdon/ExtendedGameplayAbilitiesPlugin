// Copyright Bohdon Sayre, All Rights Reserved.

#include "GameplayEffectSet.h"


bool FGameplayEffectSpecSet::HasValidEffects() const
{
	return EffectSpecs.Num() > 0;
}
