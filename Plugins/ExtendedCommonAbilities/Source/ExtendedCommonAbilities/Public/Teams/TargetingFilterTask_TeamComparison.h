// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Tasks/TargetingFilterTask_BasicFilterTemplate.h"
#include "TargetingFilterTask_TeamComparison.generated.h"


/**
 * Filter target data based on a team comparison.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API UTargetingFilterTask_TeamComparison : public UTargetingFilterTask_BasicFilterTemplate
{
	GENERATED_BODY()

public:
	UTargetingFilterTask_TeamComparison(const FObjectInitializer& ObjectInitializer);

	/** Include any non-blocking hit if the trace hit nothing. */
	UPROPERTY(EditAnywhere, Category = "Teams", meta = (Bitmask, BitmaskEnum = "/Script/AIModule.ETeamAttitude"))
	bool bIncludeNonBlockingHit = true;

	/** Only include results that match this team attitude. */
	UPROPERTY(EditAnywhere, Category = "Teams", meta = (Bitmask, BitmaskEnum = "/Script/AIModule.ETeamAttitude"))
	uint8 AttitudeMask;

	/** Only include results that match this team comparison. */
	UPROPERTY(EditAnywhere, Category = "Teams", meta = (Bitmask, BitmaskEnum = "/Script/ExtendedCommonAbilities.ECommonTeamComparison"))
	uint8 ComparisonMask;

	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
	virtual bool ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& TargetData) const override;
};
