// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectSet.h"
#include "ExtendedAbilitySystemComponent.generated.h"


/**
 * Extends the AbilitySystemComponent with support for gameplay effect sets and more.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UExtendedAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UExtendedAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Create and return an effect spec set.
	 * The spec set can then be applied using ApplyEffectContainerToSelf on this or another ability system.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayEffects")
	FGameplayEffectSpecSet MakeEffectSpecSet(const FGameplayEffectSet& EffectSet, float Level);

	/**
	 * Apply all effects from an effect spec set to this ability system.
	 * @return All active gameplay effect handles for any applied effects.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "ApplyGameplayEffectSpecSetToSelf", Category = "GameplayEffects")
	TArray<FActiveGameplayEffectHandle> ApplyGameplayEffectSpecSetToSelf(const FGameplayEffectSpecSet& EffectSpecSet);
};
