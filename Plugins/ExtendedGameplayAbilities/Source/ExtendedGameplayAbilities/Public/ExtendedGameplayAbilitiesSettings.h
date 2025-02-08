// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
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
	UExtendedGameplayAbilitiesSettings();

	UPROPERTY(Config, EditAnywhere, NoClear, Meta = (AllowAbstract = false), Category = "General")
	TSubclassOf<UGameplayEffect> DefaultDynamicCooldownEffectClass;
};
