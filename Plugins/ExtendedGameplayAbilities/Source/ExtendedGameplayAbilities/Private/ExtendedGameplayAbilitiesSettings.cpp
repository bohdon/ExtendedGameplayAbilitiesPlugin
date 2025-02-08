// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedGameplayAbilitiesSettings.h"

#include "Effects/DynamicCooldownGameplayEffect.h"


UExtendedGameplayAbilitiesSettings::UExtendedGameplayAbilitiesSettings()
{
	DefaultDynamicCooldownEffectClass = UDynamicCooldownGameplayEffect::StaticClass();
}
