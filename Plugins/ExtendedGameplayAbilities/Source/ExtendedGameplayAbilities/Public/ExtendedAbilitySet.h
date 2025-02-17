// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ExtendedAbilitySet.generated.h"

class UExtendedAbilitySet;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;


/**
 * Used by ExtendedGameplayAbilitySet to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct EXTENDEDGAMEPLAYABILITIES_API FExtendedAbilitySetAbility
{
	GENERATED_BODY()

	/** The ability to grant. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowAbstract = false))
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	/** The level of the ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	/** The input tag to use for the ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * Used by ExtendedGameplayAbilitySet to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct EXTENDEDGAMEPLAYABILITIES_API FExtendedAbilitySetEffect
{
	GENERATED_BODY()

	/** The effect to grant. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowAbstract = false))
	TSubclassOf<UGameplayEffect> Effect = nullptr;

	/** The level of the effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;
};


/**
 * Used by ExtendedGameplayAbilitySet to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct EXTENDEDGAMEPLAYABILITIES_API FExtendedAbilitySetAttributes
{
	GENERATED_BODY()

	/** The attribute set to grant. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowAbstract = false))
	TSubclassOf<UAttributeSet> AttributeSet = nullptr;
};


/**
 * Handles from the application of an ability set that can be used to remove
 * the granted abilities and effects.
 */
USTRUCT(BlueprintType)
struct EXTENDEDGAMEPLAYABILITIES_API FExtendedAbilitySetHandles
{
	GENERATED_BODY()

	/** The ability set that was granted. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UExtendedAbilitySet> AbilitySet;

	/** Handles of the granted gameplay abilities. */
	UPROPERTY(BlueprintReadOnly)
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	/** Handles of the granted gameplay effects. */
	UPROPERTY(BlueprintReadOnly)
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	/**
	 * The attribute set classes that were spawned.
	 * This will be empty if the ability system already had matching attribute sets.
	 */
	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;

	/** Additional user data for subclasses of UExtendedAbilitySet to use if needed. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> UserData;

	/** Clear all handles and references. */
	void Reset();

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);

	void AddAttributeSet(const UAttributeSet* AttributeSet);
};


/**
 * Defines a group of abilities, effects, and attribute sets to be
 * easily applied to an ability system component together.
 */
UCLASS(BlueprintType, Blueprintable)
class EXTENDEDGAMEPLAYABILITIES_API UExtendedAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Gameplay abilities to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (TitleProperty = "{Ability} Lv{Level}"))
	TArray<FExtendedAbilitySetAbility> GrantedAbilities;

	/** Gameplay effects to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (TitleProperty = "{Effect} Lv{Level}"))
	TArray<FExtendedAbilitySetEffect> GrantedEffects;

	/** Attribute sets to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (TitleProperty = "{AttributeSet}"))
	TArray<FExtendedAbilitySetAttributes> GrantedAttributes;

	/**
	 * Grant this ability set to an ability system.
	 * @param AbilitySystem The ability system to receive the abilities.
	 * @param SourceObject The object responsible for granting this ability set.
	 * @param OverrideLevel An override to control the level for all applied abilities and effects.
	 * @return Handles to all abilities, effects, and attributes that were given.
	 */
	virtual FExtendedAbilitySetHandles GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystem,
	                                                       UObject* SourceObject = nullptr,
	                                                       int32 OverrideLevel = -1) const;

	/**
	 * Remove this ability set from an ability system.
	 * @param AbilitySystem The ability system to remove from.
	 * @param AbilitySetHandles The handles to abilities and effects that were given in GiveToAbilitySystem.
	 * @param bEndImmediately End and remove abilities immediately, instead of removing them after they end naturally.
	 * @param bKeepAttributeSets If true, don't remove attribute sets (that may be needed by other effects or abilities).
	 */
	virtual void RemoveFromAbilitySystem(UAbilitySystemComponent* AbilitySystem,
	                                     FExtendedAbilitySetHandles& AbilitySetHandles,
	                                     bool bEndImmediately = false,
	                                     bool bKeepAttributeSets = false) const;

protected:
	/** Create and return a new ability spec to add to the ability system. */
	virtual FGameplayAbilitySpec CreateAbilitySpec(const FExtendedAbilitySetAbility& AbilityToGrant,
	                                               UAbilitySystemComponent* AbilitySystem,
	                                               UObject* SourceObject,
	                                               int32 OverrideLevel = -1) const;

	/** Create and return a new effect spec to add to the ability system. */
	virtual FGameplayEffectSpec CreateEffectSpec(const FExtendedAbilitySetEffect& EffectToGrant,
	                                             UAbilitySystemComponent* AbilitySystem,
	                                             UObject* SourceObject,
	                                             int32 OverrideLevel = -1) const;
};
