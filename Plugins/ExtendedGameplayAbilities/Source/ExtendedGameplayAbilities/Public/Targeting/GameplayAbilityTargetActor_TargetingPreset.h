// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Types/TargetingSystemTypes.h"
#include "GameplayAbilityTargetActor_TargetingPreset.generated.h"

class AGameplayAbilityWorldReticle;
class UTargetingPreset;


/**
 * A targeting actor that uses a TargetingPreset with the TargetingSubsystem
 * to acquire target data.
 */
UCLASS(DisplayName = "Targeting Preset")
class EXTENDEDGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_TargetingPreset : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_TargetingPreset();

	/** The targeting preset to use. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Targeting")
	TObjectPtr<UTargetingPreset> Preset;

	/**
	 * Run the targeting asynchronously. Better for complex targeting tasks,
	 * but the results may be laggy when used with a reticle without some kind of interpolation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Targeting")
	bool bAsync = false;

	/** Automatically perform targeting every update. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	bool bContinuousTargeting = true;

	/** Return true if at least one targeting request has completed, regardless of whether the target data is empty. */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool HasTargetData() const { return bHasTargetData; }

	/** Return the current target data. */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	const FGameplayAbilityTargetDataHandle& GetTargetData() const { return TargetData; }

	/** Create a targeting source context. */
	virtual FTargetingSourceContext GetTargetingContext();

	/** Return the current target request handle. */
	const FTargetingRequestHandle& GetTargetingHandle() const { return TargetingHandle; }

	/**
	 * Start a new targeting request. Does nothing if one is already active.
	 * This only needs to be called manually if bContinuousTargeting is false, otherwise targeting
	 * will be performed automatically until the actor is destroyed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void PerformTargeting();

	virtual void SpawnReticleActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual bool IsConfirmTargetingAllowed() override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	/**
	 * Handle to the current targeting request and data stores.
	 * Valid most of the time after targeting has been performed once.
	 */
	FTargetingRequestHandle TargetingHandle;

	/** The target data from the last targeting request that completed. */
	FGameplayAbilityTargetDataHandle TargetData;

	/** True when at least one targeting request has completed. */
	bool bHasTargetData = false;

	/** Is a targeting request currently in progress? */
	bool bIsRequestInProgress = false;

	TWeakObjectPtr<AGameplayAbilityWorldReticle> ReticleActor;

	/** Start a new targeting request. */
	virtual void PerformTargetingInternal(bool bInAsync);

	/** Called when an async targeting subsystem request has completed. */
	virtual void OnTargetingRequestCompleted(FTargetingRequestHandle TargetingRequestHandle);

	/** Update TargetData from the latest targeting request's results. */
	virtual void UpdateTargetData();
};
