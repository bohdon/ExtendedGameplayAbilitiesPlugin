// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ExtendedGameplayAbilitiesSettings.generated.h"

class UGameplayEffect;


/**
 * Settings for extended gameplay abilities.
 */
UCLASS(Config = "Game", DefaultConfig, DisplayName = "Extended Gameplay Abilities")
class EXTENDEDGAMEPLAYABILITIES_API UExtendedGameplayAbilitiesSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Meta = (AllowAbstract = false), Category = "General")
	TSubclassOf<UGameplayEffect> DefaultDynamicCooldownEffectClass;
};
