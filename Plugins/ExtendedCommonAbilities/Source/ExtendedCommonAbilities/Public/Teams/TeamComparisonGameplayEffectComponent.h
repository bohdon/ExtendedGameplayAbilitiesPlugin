// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "TeamComparisonGameplayEffectComponent.generated.h"


/**
 * Conditionally apply the effect based on the results of a team comparison.
 */
UCLASS(DisplayName = "Team Comparison to Apply This Effect")
class EXTENDEDCOMMONABILITIES_API UTeamComparisonGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	UTeamComparisonGameplayEffectComponent();

	/** Only apply the effect if the source has this team attitude towards the target. */
	UPROPERTY(EditAnywhere, Category = "Teams", meta = (Bitmask, BitmaskEnum = "/Script/AIModule.ETeamAttitude"))
	uint8 AttitudeMask;

	/** Only apply the effect if the source and target team have this comparison. */
	UPROPERTY(EditAnywhere, Category = "Teams", meta = (Bitmask, BitmaskEnum = "/Script/ExtendedCommonAbilities.ECommonTeamComparison"))
	uint8 ComparisonMask;

	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
};
