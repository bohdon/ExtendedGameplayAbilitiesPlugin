// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitiesInitStateComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "InitStateCharacter.h"
#include "AbilityCharacter.generated.h"

class UAbilitiesInitStateComponent;


/**
 * Base class for characters that use ability systems.
 * Uses the AbilitiesInitStateComponent to ensure ability system initialization happens
 * in a reliable way, regardless of the use of PlayerStates or other desired setups.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API AAbilityCharacter : public AInitStateCharacter,
                                                      public IGameplayTagAssetInterface,
                                                      public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAbilityCharacter(const FObjectInitializer& ObjectInitializer);

	// IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	T* GetAbilitySystemComponent() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, UAbilitySystemComponent>::Value,
		              "'T' template parameter to GetAbilitySystemComponent must be derived from UAbilitySystemComponent");
		return Cast<T>(GetAbilitySystemComponent());
	}

	UAbilitiesInitStateComponent* GetAbilitiesInitStateComponent() const;

	/** Bind to input events to press ability input by tag. */
	virtual void InputAbilityTagPressed(FGameplayTag InputTag);

	/** Bind to input events to release ability input by tag. */
	virtual void InputAbilityTagReleased(FGameplayTag InputTag);

protected:
	virtual void BeginPlay() override;

	virtual void OnInitializeAbilitySystem();
	virtual void OnUninitializeAbilitySystem();
};
