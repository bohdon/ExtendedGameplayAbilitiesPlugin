// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
