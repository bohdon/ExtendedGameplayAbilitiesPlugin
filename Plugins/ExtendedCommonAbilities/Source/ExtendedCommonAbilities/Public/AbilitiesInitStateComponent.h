// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PawnInitStateComponent.h"
#include "AbilitiesInitStateComponent.generated.h"

class UAbilitySystemComponent;


/**
 * A PawnInitStateComponent that also handles initializing an AbilitySystemComponent.
 * Initialize and Uninitialize events can be used elsewhere as a consistent point of setup and teardown.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API UAbilitiesInitStateComponent : public UPawnInitStateComponent
{
	GENERATED_BODY()

public:
	UAbilitiesInitStateComponent(const FObjectInitializer& ObjectInitializer);

	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystem; }

	/** Initialize the ability system, setting this pawn as the avatar. */
	virtual void InitializeAbilitySystem(UAbilitySystemComponent* InAbilitySystem, AActor* Owner);

	/** Uninitialize the ability system, removing this pawn as the avatar. */
	virtual void UninitializeAbilitySystem();

	/** Uninitialize the ability system, but only if our pawn is still the avatar. */
	void CleanupAbilitySystem();

	/** Called when the pawn becomes the ability system's avatar. */
	FSimpleMulticastDelegate OnAbilitySystemInitializedEvent;

	/** Called when the pawn is removed as the ability system's avatar. */
	FSimpleMulticastDelegate OnAbilitySystemUninitializedEvent;

	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual bool CheckDataAvailable(UGameFrameworkComponentManager* Manager) const override;

protected:
	/** The Ability System to use for this pawn. Cached pointer for convenience. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	/** Find the ability system and owner to use. */
	virtual void FindAbilitySystemAndOwner(UAbilitySystemComponent*& OutAbilitySystem, AActor*& OutOwner);
};
