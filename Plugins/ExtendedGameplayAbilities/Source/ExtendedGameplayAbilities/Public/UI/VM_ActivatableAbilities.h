// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "MVVMViewModelBase.h"
#include "VM_ActivatableAbilities.generated.h"

class UExtendedAbilitySystemComponent;
class UVM_GameplayAbility;


/**
 * A view model for displaying activatable abilities of an ability system.
 * Requires an ExtendedAbilitySystemComponent.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UVM_ActivatableAbilities : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	/** The owning ability system. */
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TWeakObjectPtr<UExtendedAbilitySystemComponent> AbilitySystem;

	/** Only include abilities that match this tag query. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayTagQuery AbilityTagQuery;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetAbilitySystem(UExtendedAbilitySystemComponent* NewAbilitySystem);

	UFUNCTION(BlueprintSetter)
	void SetAbilityTagQuery(const FGameplayTagQuery& NewTagQuery);

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

	UExtendedAbilitySystemComponent* GetAbilitySystem() const { return AbilitySystem.Get(); }

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

	virtual void OnGiveAbility(FGameplayAbilitySpec& GameplayAbilitySpec);
	virtual void OnRemoveAbility(FGameplayAbilitySpec& GameplayAbilitySpec);
};
