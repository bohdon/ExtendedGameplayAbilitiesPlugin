// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectSet.h"
#include "ExtendedAbilitySystemComponent.generated.h"

class UExtendedAbilitySet;


/**
 * Extends the AbilitySystemComponent with support for gameplay effect sets and more.
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class EXTENDEDGAMEPLAYABILITIES_API UExtendedAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UExtendedAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Loose gameplay tags to add to this ability system, usually character type, object type, or other traits. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	FGameplayTagContainer DefaultTags;

	/** Abilities, effects, and attribute sets to grant at startup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	TArray<TObjectPtr<UExtendedAbilitySet>> StartupAbilitySets;

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

	virtual void InitializeComponent() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	/** Called when ability input has been pressed by tag. */
	void AbilityTagInputPressed(const FGameplayTag& InputTag);

	/** Called when ability input has been released by tag. */
	void AbilityTagInputReleased(const FGameplayTag& InputTag);

	/** Sends a local player Input Pressed event by input tag, notifying any bound abilities. */
	UFUNCTION(BlueprintCallable, Meta = (AutoCreateRefTerm = "InputTag"), Category = "Gameplay Abilities")
	void PressInputTag(const FGameplayTag& InputTag);

	/** Sends a local player Input Released event by input tag, notifying any bound abilities. */
	UFUNCTION(BlueprintCallable, Meta = (AutoCreateRefTerm = "InputTag"), Category = "Gameplay Abilities")
	void ReleaseInputTag(const FGameplayTag& InputTag);

	DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityAddOrRemoveDelegate, FGameplayAbilitySpec& /*AbilitySpec*/);

	/** Called when a new ability is added. */
	FAbilityAddOrRemoveDelegate OnGiveAbilityEvent;

	/** Called when an ability is removed. */
	FAbilityAddOrRemoveDelegate OnRemoveAbilityEvent;
};
