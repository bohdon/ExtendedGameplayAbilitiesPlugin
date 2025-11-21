// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Components/PawnComponent.h"
#include "PawnAbilityInputComponent.generated.h"

class UEnhancedInputComponent;
class UExtendedAbilitySystemComponent;
class UGameplayTagInputConfig;


/**
 * Handles setting up input bindings for a controlled pawn with abilities.
 * Makes use of UGameplayTagInputConfig to map input actions to tag-based ability input.
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class EXTENDEDCOMMONABILITIES_API UPawnAbilityInputComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPawnAbilityInputComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	/**
	 * Bind default input for the pawn and ability system.
	 * If a UAbilitiesInitStateComponent is present this will be called automatically
	 * after the ability system is initialized.
	 */
	virtual void SetupPlayerInput(UInputComponent* PlayerInputComponent);

	/** Adds a contextual input config. */
	virtual void AddInputConfig(const UGameplayTagInputConfig* InputConfig);

	/** Removes a contextual input config if it has been added. */
	virtual void RemoveInputConfig(const UGameplayTagInputConfig* InputConfig);

protected:
	virtual void OnAbilitySystemInitialized();

	virtual void BindAbilityActions(UEnhancedInputComponent* EnhancedInput, const UGameplayTagInputConfig* InputConfig);

	/**
	 * Bind to input Triggered events to press ability input by tag.
	 * Checks if this is the first Triggered event and calls InputAbilityTagPressed if so.
	 */
	virtual void OnAbilityInputTagTriggered(const FInputActionInstance& InputActionInstance, FGameplayTag InputTag);

	/** Called when ability input actions are triggered to send ability input by tag. */
	virtual void OnAbilityInputTagPressed(FGameplayTag InputTag);

	/** Called when ability input actions are completed. */
	virtual void OnAbilityInputTagReleased(FGameplayTag InputTag);

public:
	/**
	 * Default input config for core abilities. Additional configs can be added at runtime.
	 * InputAbilityTagPressed/Released will be called based on the matching input actions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TObjectPtr<UGameplayTagInputConfig> DefaultInputConfig;

	/**
	 * When true, send every Triggered input event to ability system, instead of just the first.
	 * Otherwise, ability input will only be sent on first trigger, and on completion.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bSendContinuousTriggers = false;

protected:
	/** Used to detect the first time an input tag action is triggered. */
	TArray<FGameplayTag> TriggeredInputTags;

	/** Cached reference to the ability system. */
	UPROPERTY(Transient)
	TObjectPtr<UExtendedAbilitySystemComponent> AbilitySystem;
};
