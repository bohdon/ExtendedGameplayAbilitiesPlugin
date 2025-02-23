// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Templates/SubclassOf.h"
#include "AbilitySystemViewModelBase.generated.h"

class UAbilitySystemComponent;


/**
 * Base class for a view model that uses a UAbilitySystemComponent.
 *
 * Subclasses should implement PreSystemChange and PostSystemChange to unbind/bind relevant events,
 * as well as call those methods before and after changing other properties that affect the bindings.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UAbilitySystemViewModelBase : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	/** The owning ability system. */
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

	/**
	 * If set, the required class of ability system for this view model.
	 * An error will occur if calling SetAbilitySystem with an invalid ability system class.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UAbilitySystemComponent> RequireAbilitySystemClass;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem);

	UFUNCTION(BlueprintPure, FieldNotify)
	UAbilitySystemComponent* GetAbilitySystem() const { return AbilitySystem.Get(); }

	template <class T>
	T* GetAbilitySystem() const
	{
		static_assert(TIsDerivedFrom<T, UAbilitySystemComponent>::Value, "T must be derived from UAbilitySystemComponent");
		return Cast<T>(GetAbilitySystem());
	}

protected:
	/**
	 * Called before changing the ability system or any other relevant properties.
	 * Used to unbind events.
	 */
	virtual void PreSystemChange();

	/**
	 * Called after the ability system or any other relevant properties are changed.
	 * Used to bind events and broadcast field value changes.
	 */
	virtual void PostSystemChange();
};
