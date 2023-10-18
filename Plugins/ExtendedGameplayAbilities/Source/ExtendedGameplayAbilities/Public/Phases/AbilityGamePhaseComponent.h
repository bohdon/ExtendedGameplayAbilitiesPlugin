// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AbilityGamePhaseComponent.generated.h"

class UAbilitySystemComponent;
class UGamePhaseAbility;


DECLARE_LOG_CATEGORY_EXTERN(LogAbilityGamePhase, Log, All);


/**
 * Handles managing game phase abilities on an ability system using tags.
 * Matching game phases can be run simultaneously, but mismatching ones will be canceled when a new phase begins.
 *
 * Example: While GamePhase.Playing is active, GamePhase.Playing.SuddenDeath can begin without ending GamePhase.Playing,
 * but once GamePhase.PostGame activates, all GamePhase.Playing.* abilities will be ended.
 *
 * This is largely modeled after the ULyraGamePhaseSubsystem.
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class EXTENDEDGAMEPLAYABILITIES_API UAbilityGamePhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbilityGamePhaseComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * Set the ability system that will handle running game phase abilities.
	 * Intended to be called during PostInitializeComponents in the owning actor, usually a GameState.
	 */
	void SetAbilitySystem(UAbilitySystemComponent* InAbilitySystem);

	UFUNCTION(BlueprintCallable, Category = "Ability|GamePhases")
	void StartPhase(TSubclassOf<UGamePhaseAbility> PhaseAbility);

	UFUNCTION(BlueprintPure, Meta = (Categories = "GamePhase"), Category = "Ability|GamePhases")
	bool IsPhaseActive(FGameplayTag PhaseTag);

	virtual void OnBeginPhase(const UGamePhaseAbility* Ability, const FGameplayAbilitySpecHandle AbilityHandle);

	virtual void OnEndPhase(const UGamePhaseAbility* Ability, const FGameplayAbilitySpecHandle AbilityHandle);

protected:
	/** Entry for storing data about an active phase ability. */
	struct FActiveGamePhaseEntry
	{
		FGameplayTag PhaseTag;
	};

	/** Map of the currently active phases, by ability spec handle. */
	TMap<FGameplayAbilitySpecHandle, FActiveGamePhaseEntry> ActivePhases;

	/** Cached pointer to the ability system that handles running game phases. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	void CancelAbilitiesForNewPhase(const FGameplayTag& NewPhaseTag);
};
