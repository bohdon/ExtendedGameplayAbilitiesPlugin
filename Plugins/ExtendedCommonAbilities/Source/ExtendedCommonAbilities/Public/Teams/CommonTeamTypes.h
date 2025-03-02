// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "CommonTeamTypes.generated.h"


/**
 * Result of comparing two object's teams.
 */
UENUM(BlueprintType)
enum class ECommonTeamComparison : uint8
{
	/** Both objects are on the same team. */
	SameTeam,
	/** The objects are on different (but valid) teams. */
	DifferentTeams,
	/** One or both of the objects has no team. */
	NoTeam,
};

class FCommonTeamTypes
{
public:
	static uint8 AllAttitudesMask;
	static uint8 AllComparisonsMask;

	FORCEINLINE static bool MatchesAttitudeMask(const ETeamAttitude::Type Attitude, const uint8 AttitudeMask)
	{
		return !!((1 << Attitude) & AttitudeMask);
	}

	FORCEINLINE static bool MatchesComparisonMask(const ECommonTeamComparison Comparison, const uint8 ComparisonMask)
	{
		return !!((1 << static_cast<uint8>(Comparison)) & ComparisonMask);
	}
};
