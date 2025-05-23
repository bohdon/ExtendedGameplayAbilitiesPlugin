﻿// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "ExtendedAbilitySet.h"
#include "GameplayEffectSet.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Targeting/ExtendedTargetingFilterTask_SingleResult.h"
#include "ExtendedAbilitySystemStatics.generated.h"

class AGameplayAbilityTargetActor_TargetingPreset;
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

	/** Create target data from a literal transform. */
	UFUNCTION(BlueprintPure, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle AbilityTargetDataFromTransform(const FTransform Transform);

	/** Return the transform results for a targeting request handle */
	UFUNCTION(BlueprintPure, Category = "Targeting System|Targeting Results")
	static void GetTargetingResultsTransforms(FTargetingRequestHandle TargetingHandle, TArray<FTransform>& Targets);

	/** Get the targeting actor that is the source object of a targeting request. */
	UFUNCTION(BlueprintCallable, Category = "Targeting System|Targeting Results")
	static AGameplayAbilityTargetActor_TargetingPreset* GetTargetingActorForRequest(FTargetingRequestHandle TargetingHandle);

	/** Set a gameplay tag Set By Caller magnitude value for all effects in a set. */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Assign Tag Set By Caller Magnitude (For Set)"), Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecSet AssignTagSetByCallerMagnitudeForSet(FGameplayEffectSpecSet EffectSpecSet, FGameplayTag DataTag, float Magnitude);

	/** Add a hit result to the context for all effects in a set. */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Add Context Hit Result (For Set)"), Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecSet AddContextHitResultForSet(FGameplayEffectSpecSet EffectSpecSet, FHitResult HitResult, bool bReset);

	/** Set the origin of the context for all effects in a set. */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Set Context Origin (For Set)"), Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecSet SetContextOriginForSet(FGameplayEffectSpecSet EffectSpecSet, FVector Origin);

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

	/** Return all asset tags for an active gameplay effect. */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayTagContainer GetAllActiveEffectAssetTags(FActiveGameplayEffectHandle ActiveHandle);

	/** Return all granted tags for an active gameplay effect. */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayTagContainer GetAllActiveEffectGrantedTags(FActiveGameplayEffectHandle ActiveHandle);

	/**
	 * Grant an ability set to an ability system.
	 * @param AbilitySystem The ability system to receive the abilities.
	 * @param AbilitySet The ability set to grant.
	 * @param SourceObject The object responsible for granting the ability set.
	 * @param OverrideLevel An override to control the level for all applied abilities and effects.
	 * @return Handles to all abilities, effects, and attributes that were given.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Gameplay Abilities")
	static FExtendedAbilitySetHandles GiveAbilitySet(UAbilitySystemComponent* AbilitySystem,
	                                                 UExtendedAbilitySet* AbilitySet,
	                                                 UObject* SourceObject = nullptr,
	                                                 int32 OverrideLevel = -1);

	/**
	 * Remove an ability set from an ability system.
	 * This may remove an attribute set that could still be important to some other abilities / effects,
	 * so be sure that those attributes are no longer needed before removing, or set bKeepAttributeSets to true.
	 * @param AbilitySystem The ability system to update.
	 * @param AbilitySetHandles The ability set handles stored from when the ability set was given.
	 * @param bEndImmediately End and remove abilities immediately, instead of removing them after they end naturally.
	 * @param bKeepAttributeSets Don't remove granted AttributeSets.
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Abilities")
	static void RemoveAbilitySet(UAbilitySystemComponent* AbilitySystem,
	                             UPARAM(ref) FExtendedAbilitySetHandles& AbilitySetHandles,
	                             bool bEndImmediately = false,
	                             bool bKeepAttributeSets = false);

	/**
	 * Change an attribute proportionally based on the change to another attribute.
	 * This is useful for example if MaxHP increases or decreases, and you want HP to remain the same % of MaxHP as it was before the change.
	 * Intended to be called in UAttributeSet::PostAttributeChange when the max attribute has changed.
	 * @param AttributeSet The attribute set containing the attribute.
	 * @param Attribute The attribute whose value will be adjusted.
	 * @param OldRelatedValue The old value of the related attribute
	 * @param NewRelatedValue The new value of the related attribute
	 * @param bRound Round the value to an integer.
	 * @param bClamp Clamp the value to not exceed the new related attribute's value.
	 */
	static void AdjustProportionalAttribute(UAttributeSet* AttributeSet, const FGameplayAttribute& Attribute,
	                                        float OldRelatedValue, float NewRelatedValue,
	                                        bool bRound = true, bool bClamp = true);

	/** Return the base value for an attribute using an attribute set. */
	static float GetNumericAttributeBase(const UAttributeSet* AttributeSet, const FGameplayAttribute& Attribute);

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
