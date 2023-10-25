// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectSet.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExtendedAbilitySystemStatics.generated.h"

class UAbilityGamePhaseComponent;
class UExtendedAbilitySystemComponent;


/**
 * Additional static functions for working with ability systems.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UExtendedAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Tries to find an extended ability system component on the actor, will use AbilitySystemInterface or fall back to a component search. */
	UFUNCTION(BlueprintPure, Meta = (DefaultToSelf = "Actor"), Category = "Ability")
	static UExtendedAbilitySystemComponent* GetExtendedAbilitySystemComponent(AActor* Actor);

	/** Set a gameplay tag Set By Caller magnitude value for all effects in a set. */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Assign Tag Set By Caller Magnitude (For Set)"), Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecSet AssignTagSetByCallerMagnitudeForSet(FGameplayEffectSpecSet EffectSpecSet, FGameplayTag DataTag, float Magnitude);

	/**
	 * Apply a gameplay effect spec set to an actor, if it has an ability system.
	 * @param Actor The actor to apply effects to.
	 * @param EffectSpecSet The set of gameplay effect specs to apply.
	 * @param SourceObject The source object, usually projectile or gameplay object applying the effect.
	 * @param bSuccess True if the actor had an ability system, regardless of which effects were applied.
	 * @return The effect handles of all duration-based applied effects.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static TArray<FActiveGameplayEffectHandle> ApplyEffectSpecSetToActor(AActor* Actor, const FGameplayEffectSpecSet& EffectSpecSet,
	                                                                     const UObject* SourceObject, bool& bSuccess);

	/**
	 * Apply a gameplay effect spec set to an actor, but only if it has not already been affected.
	 * The array of previously affected actors is updated accordingly and used to track this info.
	 * @param Actor The actor to apply effects to.
	 * @param EffectSpecSet The set of gameplay effect specs to apply.
	 * @param SourceObject The source object, usually projectile or gameplay object applying the effect.
	 * @param bSuccess True if the actor had an ability system, regardless of which effects were applied.
	 * @param AffectedActors List of previously affected actors, if present, the actor will not have effects applied again.
	 * @return The effect handles of all duration-based applied effects.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static TArray<FActiveGameplayEffectHandle> ApplyEffectSpecSetToActorOnce(AActor* Actor, const FGameplayEffectSpecSet& EffectSpecSet,
	                                                                         const UObject* SourceObject,
	                                                                         UPARAM(Ref) TArray<AActor*>& AffectedActors,
	                                                                         bool& bSuccess);

	/**
	 * Remove any active gameplay effects from an actor that were applied from a source object.
	 * Also remove the actor from the AffectedActors array, so that ApplyEffectSpecSetToActorOnce
	 * could be called again after this to re-apply the effects.
	 *
	 * This is useful for AOE type effects where the actor should be affected while in the volume,
	 * and the effects removed once leaving that volume.
	 * @param Actor The actor to remove effects from.
	 * @param SourceObject The object that originally applied the effect, usually a projectile or other gameplay object.
	 * @param AffectedActors List of actors that have been affected previously. If present, the actor will be removed from this list.
	 * @return The number of effects that were removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static int32 RemoveEffectsFromActorBySourceObject(AActor* Actor, const UObject* SourceObject,
	                                                  UPARAM(Ref) TArray<AActor*>& AffectedActors);

	/** Return all active effects that are responsible for adding a gameplay cue. */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static TArray<FActiveGameplayEffectHandle> GetActiveEffectsGrantingGameplayCue(UAbilitySystemComponent* AbilitySystem, FGameplayTag GameplayCueTag);

	/**
	 * Change an attribute proportionally based on the change to another attribute.
	 * This is useful for example if MaxHP increases or decreases, and you want HP to remain the same % of MaxHP as it was before the change.
	 * Intended to be called in UAttributeSet::PostAttributeChange when the max attribute has changed.
	 * @param AbilitySystem The ability system where the attribute exists.
	 * @param Attribute The attribute whose value will be adjusted.
	 * @param OldRelatedValue The old value of the related attribute
	 * @param NewRelatedValue The new value of the related attribute
	 * @param bRound Round the value to an integer.
	 * @param bClamp Clamp the value to not exceed the new related attribute's value.
	 */
	static void AdjustProportionalAttribute(UAbilitySystemComponent* AbilitySystem, const FGameplayAttribute& Attribute,
	                                        float OldRelatedValue, float NewRelatedValue,
	                                        bool bRound = true, bool bClamp = true);

	/**
	 * Return the value of a data registry curve at an input time/level/value.
	 * Return the DefaultValue if the curve is not found.
	 */
	UFUNCTION(BlueprintPure, Meta = (AdvancedDisplay = "2"))
	static float GetDataRegistryValue(FDataRegistryId Id, float InputValue = 0.f, float DefaultValue = 0.f);

	/** Return the UAbilityGamePhaseComponent from the game state. */
	UFUNCTION(BlueprintPure, Meta = (WorldContext = "WorldContextObject"), Category = "Ability|GamePhases")
	static UAbilityGamePhaseComponent* GetAbilityGamePhaseComponent(const UObject* WorldContextObject);
};
