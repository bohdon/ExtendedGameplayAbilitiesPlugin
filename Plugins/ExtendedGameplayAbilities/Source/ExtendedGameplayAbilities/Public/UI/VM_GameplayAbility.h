// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "MVVMViewModelBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "VM_GameplayAbility.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
struct FGameplayAbilitySpec;


/**
 * A view model representing a gameplay ability granted to an ability system.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UVM_GameplayAbility : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	/** The owning ability system. */
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

public:
	UFUNCTION(BlueprintSetter)
	virtual void SetAbilitySpecHandle(FGameplayAbilitySpecHandle NewAbilitySpecHandle);

	UFUNCTION(BlueprintCallable)
	virtual void SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem);

	UAbilitySystemComponent* GetAbilitySystem() const { return AbilitySystem.Get(); }

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
	 * Return the ability class default object, which should never be modified, but can be used
	 * to retrieve information about the ability and its class.
	 */
	UFUNCTION(BlueprintPure, FieldNotify)
	const UGameplayAbility* GetAbilityCDO() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	TSubclassOf<UGameplayAbility> GetAbilityClass() const;

	FGameplayAbilitySpec* GetAbilitySpec() const;

protected:
	/** Cooldown tags that were registered for change events. */
	FGameplayTagContainer RegisteredCooldownTags;

	/**
	 * True during OnAnyAbilityActivated, and used by IsActive to temporarily return true,
	 * since the ability spec ActiveCount isn't updated until after that event.
	 */
	bool bIsActivating = false;

	virtual void OnAnyAbilityActivated(UGameplayAbility* GameplayAbility);
	virtual void OnAnyAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	virtual void OnCooldownTagChanged(FGameplayTag GameplayTag, int32 NewCount);
};
