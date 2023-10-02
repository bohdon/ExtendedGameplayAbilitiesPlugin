// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "GameplayEffectSet.generated.h"

class UGameplayEffect;


/**
 * Represents a group of gameplay effects that can be applied together on a target.
 * This concept is demonstrated in Epic's Action RPG Sample:
 * https://www.unrealengine.com/marketplace/en-US/product/action-rpg
 * ...and talked about in the tranek GAS documentation:
 * https://github.com/tranek/GASDocumentation
 */
USTRUCT(BlueprintType)
struct EXTENDEDGAMEPLAYABILITIES_API FGameplayEffectSet
{
	GENERATED_BODY()

	FGameplayEffectSet()
	{
	}

	/** All gameplay effect classes in this set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|GameplayEffectSets")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	/** Map of default set-by-caller values to use for all effects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|GameplayEffectSets")
	TMap<FGameplayTag, FScalableFloat> SetByCallerMagnitudes;
};


/**
 * Represents a group of gameplay effect specs ready to be applied to a target.
 */
USTRUCT(BlueprintType)
struct EXTENDEDGAMEPLAYABILITIES_API FGameplayEffectSpecSet
{
	GENERATED_BODY()

	FGameplayEffectSpecSet()
	{
	}

	/** Gameplay effect specs in this set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|GameplayEffectSets")
	TArray<FGameplayEffectSpecHandle> EffectSpecs;

	/** Return true if this set has any valid effect specs. */
	bool HasValidEffects() const;
};
