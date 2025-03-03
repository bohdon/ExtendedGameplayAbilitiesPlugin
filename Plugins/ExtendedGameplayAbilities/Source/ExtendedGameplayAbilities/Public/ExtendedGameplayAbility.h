// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "GameplayEffectSet.h"
#include "Abilities/GameplayAbility.h"
#include "ExtendedGameplayAbility.generated.h"

class ACharacter;
class APawn;
class UInputComponent;
class UInputMappingContext;


/**
 * Defines a dynamic cooldown for an extended gameplay ability.
 */
USTRUCT(BlueprintType)
struct FDynamicGameplayAbilityCooldown
{
	GENERATED_BODY()

	FDynamicGameplayAbilityCooldown()
	{
	}

	/** The duration of the cooldown. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "0"))
	FScalableFloat Duration;

	/** The cooldown tags. Replaces all tags from the cooldown effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;

	/**
	 * The dynamic cooldown gameplay effect. Must have a duration policy of HasDuration and no tags.
	 * This can be defined once per project in the extended gameplay abilities plugin settings.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectClass;
};


/**
 * Extends the base gameplay ability with a reusable gameplay effect set map, functions
 * for working with effect sets, and other optional features like auto activation when granted.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UExtendedGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UExtendedGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Automatically activate this ability when granted to the ability system.
	 * Useful for passives or inflicted abilities.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Triggers")
	bool bActivateWhenGranted = false;

	/** Setup an input component for this ability when activated, to support input actions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	bool bEnableInputBindings = false;

	/** The priority if the input component for this ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "bEnableInputBindings"))
	int32 InputPriority = 0;

	/** Whether to block input to lower-priority input components. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "bEnableInputBindings"))
	bool bBlockInput = false;

	/** If true, use a cooldown duration defined by this ability, optionally via function. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (InlineEditConditionToggle), Category = "Cooldowns")
	bool bHasDynamicCooldown = false;

	/**
	 * When enabled, allows specifying a cooldown duration and tags per-ability. You can then also implement
	 * the GetDynamicCooldownDuration to provide a variable cooldown based on gameplay logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (EditCondition = "bHasDynamicCooldown"), Category = "Cooldowns")
	FDynamicGameplayAbilityCooldown DynamicCooldown;

	/**
	 * Optional map of effect sets on this ability by tag, allowing gameplay logic to apply
	 * effects generically, where subclasses can easily modify the map to change which effects to apply.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayEffects")
	TMap<FGameplayTag, FGameplayEffectSet> EffectSetMap;

	/**
	 * Return the duration to use a for a dynamic cooldown.
	 * By default this returns the duration defined in DynamicCooldown, but can be implemented to
	 * provide a variable cooldown based on gameplay logic.
	 */
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Cooldowns")
	float GetDynamicCooldownDuration(int32 AbilityLevel) const;

	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                         const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	                         const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	                                               const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
	                                               FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Called when the avatar of the owning ability system has been set. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnAvatarSet", Category = "Ability")
	void OnAvatarSet_BP();

	/** Return an effect set from the EffectSetMap by tag. */
	UFUNCTION(BlueprintPure, Category = "Ability|GameplayEffect")
	FGameplayEffectSet GetEffectSet(FGameplayTag Tag) const;

	/** Create a new gameplay effect spec set. */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	FGameplayEffectSpecSet MakeEffectSpecSet(const FGameplayEffectSet& EffectSet, int32 OverrideGameplayLevel = -1);

	/** Create a new gameplay effect spec set using a set from the EffectSetMap by tag. */
	UFUNCTION(BlueprintPure, Meta = (AdvancedDisplay = "1"), Category = "Ability|GameplayEffect")
	FGameplayEffectSpecSet MakeEffectSpecSetByTag(FGameplayTag Tag, int32 OverrideGameplayLevel = -1);

	/** Apply all effects in a spec set to the owning ability system. */
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "ApplyEffectSpecSetToOwner"), Category = "Ability|GameplayEffect")
	TArray<FActiveGameplayEffectHandle> ApplyEffectSpecSetToOwner_BP(const FGameplayEffectSpecSet& EffectSpecSet);

	/**
	 * Apply all effects in a spec set to the owning ability system.
	 * Safe to call on CDO/NonInstance abilities.
	 */
	TArray<FActiveGameplayEffectHandle> ApplyEffectSpecSetToOwner(const FGameplayAbilitySpecHandle AbilityHandle,
	                                                              const FGameplayAbilityActorInfo* ActorInfo,
	                                                              const FGameplayAbilityActivationInfo ActivationInfo,
	                                                              const FGameplayEffectSpecSet& EffectSpecSet);

	/** Return the value of a curve table at this ability's level. */
	UFUNCTION(BlueprintPure, Meta = (HideSelfPin = true, AdvancedDisplay = "1"), Category = "Ability")
	float GetAbilityStat(FDataRegistryId Id, float DefaultValue = 0.f) const;

	/** Return the avatar as a pawn from the actor info. */
	UFUNCTION(BlueprintPure, Category = "Ability")
	APawn* GetPawnFromActorInfo() const;

	/** Return the avatar as a Character from the actor info. */
	UFUNCTION(BlueprintPure, Category = "Ability")
	ACharacter* GetCharacterFromActorInfo() const;

	/** Return the controller from the actor info. */
	UFUNCTION(BlueprintPure, Category = "Ability")
	AController* GetControllerFromActorInfo() const;

	/** Return the player controller from the actor info. */
	UFUNCTION(BlueprintPure, Category = "Ability")
	APlayerController* GetPlayerControllerFromActorInfo() const;

	/** Set new ability state tags for this ability. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetAbilityStateTags(FGameplayTagContainer NewStateTags);

	/** Add an ability state tag to this ability. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void AddAbilityStateTag(FGameplayTag StateTag);

	/** Remove an ability state tag from this ability. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void RemoveAbilityStateTag(FGameplayTag StateTag);

	/** Clear all ability state tags on this ability. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ClearAbilityStateTags();

	/** Return the ability's current state tags. */
	UFUNCTION(BlueprintPure)
	const FGameplayTagContainer& GetAbilityStateTags() const { return AbilityStateTags; }

	/**
	 * Add an input mapping context, only on the locally controlled client.
	 * Will be removed automatically when the ability deactivates, if not removed manually.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Input", meta = (AutoCreateRefTerm = "Options"))
	void AddInputMappingContext(const UInputMappingContext* MappingContext, int32 Priority, const FModifyContextOptions& Options = FModifyContextOptions());

	UFUNCTION(BlueprintCallable, Category = "Ability|Input", meta = (AutoCreateRefTerm = "Options"))
	void RemoveInputMappingContext(const UInputMappingContext* MappingContext, const FModifyContextOptions& Options = FModifyContextOptions());

protected:
	/**
	 * Dynamically added tags that this ability has based on its current state.
	 * These can be changed during execution and used in combination with CancelAbilitiesWithState tags.
	 * Not replicated.
	 */
	UPROPERTY(Transient)
	FGameplayTagContainer AbilityStateTags;

	/** Input component used to support enhanced input events directly in the ability. */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UInputComponent> InputComponent;

	/** Mapping contexts that were added while the ability was active. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<const UInputMappingContext>> ActiveMappingContexts;

	virtual void InitializeInputComponent();
	virtual void UninitializeInputComponent();
};
