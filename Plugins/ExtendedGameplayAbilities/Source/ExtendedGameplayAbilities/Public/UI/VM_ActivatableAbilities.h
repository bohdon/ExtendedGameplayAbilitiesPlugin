// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemViewModelBase.h"
#include "GameplayAbilitySpec.h"
#include "VM_ActivatableAbilities.generated.h"

class UExtendedAbilitySystemComponent;
class UVM_GameplayAbility;


/**
 * A view model for displaying activatable abilities of an ability system.
 * Requires an ExtendedAbilitySystemComponent.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UVM_ActivatableAbilities : public UAbilitySystemViewModelBase
{
	GENERATED_BODY()

protected:
	/** Only include abilities that match this tag query. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayTagQuery AbilityTagQuery;

public:
	UVM_ActivatableAbilities();

	UFUNCTION(BlueprintSetter)
	void SetAbilityTagQuery(const FGameplayTagQuery& NewTagQuery);

	const FGameplayTagQuery& GetAbilityTagQuery() const { return AbilityTagQuery; }

	/** Return ability spec handles for each activatable ability. */
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<FGameplayAbilitySpecHandle> GetAbilitySpecHandles() const;

	/** Return an array of view models for each activatable ability. */
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<UVM_GameplayAbility*> GetAbilityViewModels() const;

	/** Return the first ability spec handle that matches tag requirements. */
	UFUNCTION(BlueprintPure)
	FGameplayAbilitySpecHandle FindAbilityMatchingTags(FGameplayTagContainer MatchingTags, FGameplayTagContainer IgnoreTags) const;

	virtual bool ShouldIncludeAbility(const FGameplayAbilitySpec& AbilitySpec) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilitiesChangedDynDelegate);

	/** Called when any ability is added or removed. */
	UPROPERTY(BlueprintAssignable)
	FAbilitiesChangedDynDelegate OnAbilitiesChangedEvent;

protected:
	/**
	 * Temporary handle to the ability being removed in OnRemoveAbility, since it won't have
	 * been removed from ActivatableAbilities yet, and needs to be ignored explicitly.
	 */
	FGameplayAbilitySpecHandle AbilityBeingRemoved;

	virtual void PreSystemChange() override;
	virtual void PostSystemChange() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& GameplayAbilitySpec);
	virtual void OnRemoveAbility(FGameplayAbilitySpec& GameplayAbilitySpec);
};
