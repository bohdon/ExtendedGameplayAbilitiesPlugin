// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "ExtendedGameplayAbility.h"
#include "GamePhaseAbility.generated.h"

class UAbilityGamePhaseComponent;


/**
 * An ability that represents a game phase. Automatically notifies UAbilityGamePhaseComponent
 * when activating and ending to properly update other phases and trigger phase callbacks.
 * This ability should only be activated through the game phase component.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UGamePhaseAbility : public UExtendedGameplayAbility
{
	GENERATED_BODY()

public:
	UGamePhaseAbility(const FObjectInitializer& ObjectInitializer);

	/**
	 * The game phase that this ability is part of. Abilities with a matching phase can coexist,
	 * but when a different phase begins, other phase abilities will be ended.
	 *
	 * Example: While GamePhase.Playing is active, GamePhase.Playing.SuddenDeath can begin without ending GamePhase.Playing,
	 * but once GamePhase.PostGame activates, all GamePhase.Playing.* abilities will be ended.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GamePhase"), Category = "GamePhase")
	FGameplayTag PhaseTag;

	/** Return the game phase component to notify when this ability activates and ends. */
	virtual UAbilityGamePhaseComponent* GetAbilityGamePhaseComponent(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
