// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "InitStateCharacter.h"
#include "AbilityCharacter.generated.h"

class UAbilitiesInitStateComponent;
class UGameplayTagInputConfig;
class UPawnAbilityInputComponent;


/**
 * Base class for characters that use ability systems.
 * Uses the AbilitiesInitStateComponent to ensure ability system initialization happens
 * in a reliable way, regardless of the use of PlayerStates or other desired setups.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API AAbilityCharacter
	: public AInitStateCharacter,
	  public IGameplayTagAssetInterface,
	  public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Component that handles binding input to the ability system via tag input config. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = "Input")
	TObjectPtr<UPawnAbilityInputComponent> PawnAbilityInputComponent;

public:
	AAbilityCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;
	virtual void PostInitializeComponents() override;

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
		static_assert(TIsDerivedFrom<T, UAbilitySystemComponent>::IsDerived, "T must derive from UAbilitySystemComponent");
		return Cast<T>(GetAbilitySystemComponent());
	}

	UAbilitiesInitStateComponent* GetAbilitiesInitStateComponent() const;

	UPawnAbilityInputComponent* GetPawnAbilityInputComponent() const { return PawnAbilityInputComponent; }

protected:
	virtual void OnInitializeAbilitySystem();
	virtual void OnUninitializeAbilitySystem();

	/** DEPRECATED: Use UPawnAbilityInputComponent::DefaultInputConfig instead. */
	UPROPERTY()
	TArray<TObjectPtr<UGameplayTagInputConfig>> AbilityInputConfigs_DEPRECATED;

	static FName PawnAbilityInputComponentName;
};
