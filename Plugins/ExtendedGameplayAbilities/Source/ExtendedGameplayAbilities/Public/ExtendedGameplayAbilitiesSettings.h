// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "ExtendedGameplayAbilitiesSettings.generated.h"

class UGameplayEffect;


/**
 * Settings for extended gameplay abilities.
 */
UCLASS(Config = "Game", DefaultConfig, DisplayName = "Extended Gameplay Abilities")
class EXTENDEDGAMEPLAYABILITIES_API UExtendedGameplayAbilitiesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UExtendedGameplayAbilitiesSettings();

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }

	UPROPERTY(Config, EditAnywhere, NoClear, Meta = (AllowAbstract = false), Category = "General")
	TSubclassOf<UGameplayEffect> DefaultDynamicCooldownEffectClass;
};
