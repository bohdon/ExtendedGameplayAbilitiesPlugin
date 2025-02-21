// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemViewModelBase.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "VM_GameplayAbility.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
struct FGameplayAbilitySpec;


/**
 * A view model representing a gameplay ability granted to an ability system.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UVM_GameplayAbility : public UAbilitySystemViewModelBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayAbilitySpecHandle AbilitySpecHandle;

public:
	UFUNCTION(BlueprintSetter)
	virtual void SetAbilitySpecHandle(FGameplayAbilitySpecHandle NewAbilitySpecHandle);

	UFUNCTION(BlueprintCallable)
	virtual void SetAbilitySystemAndSpecHandle(UAbilitySystemComponent* NewAbilitySystem, FGameplayAbilitySpecHandle NewAbilitySpecHandle);

	/** Does the ability spec handle point to a valid ability? */
	UFUNCTION(BlueprintPure, FieldNotify)
	bool HasAbility() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	bool IsActive() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	bool IsOnCooldown() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	FGameplayTagContainer GetCooldownTags() const;

	/**
	 * Get the currently active cooldown gameplay effect for this ability, if any.
	 * If multiple are active, return the one that will end last.
	 */
	UFUNCTION(BlueprintPure, FieldNotify)
	FActiveGameplayEffectHandle GetActiveCooldownEffect() const;

	/**
	 * Return the ability class default object, which should never be modified, but can be used
	 * to retrieve information about the ability and its class.
	 */
	UFUNCTION(BlueprintPure, FieldNotify)
	const UGameplayAbility* GetAbilityCDO() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	TSubclassOf<UGameplayAbility> GetAbilityClass() const;

	FGameplayAbilitySpec* GetAbilitySpec() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownEffectAppliedDynDelegate, FActiveGameplayEffectHandle, EffectHandle);

	/** Called when a cooldown effect for this ability is applied. */
	UPROPERTY(BlueprintAssignable)
	FCooldownEffectAppliedDynDelegate OnCooldownEffectAppliedEvent;

protected:
	/** Cooldown tags that were registered for change events. */
	FGameplayTagContainer RegisteredCooldownTags;

	/**
	 * True during OnAnyAbilityActivated, and used by IsActive to temporarily return true,
	 * since the ability spec ActiveCount isn't updated until after that event.
	 */
	bool bIsActivating = false;

	virtual void PreSystemChange() override;
	virtual void PostSystemChange() override;
	virtual void OnAnyAbilityActivated(UGameplayAbility* GameplayAbility);
	virtual void OnAnyAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	virtual void OnCooldownTagChanged(FGameplayTag GameplayTag, int32 NewCount);
	virtual void OnActiveGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent,
	                                         const FGameplayEffectSpec& GameplayEffectSpec,
	                                         FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
