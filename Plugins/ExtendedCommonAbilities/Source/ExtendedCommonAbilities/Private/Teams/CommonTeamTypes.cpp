// Copyright Bohdon Sayre, All Rights Reserved.


#include "Teams/CommonTeamTypes.h"

#include "GenericTeamAgentInterface.h"


uint8 FCommonTeamTypes::AllAttitudesMask =
	(1 << ETeamAttitude::Friendly) |
	(1 << ETeamAttitude::Neutral) |
	(1 << ETeamAttitude::Hostile);

uint8 FCommonTeamTypes::AllComparisonsMask =
	(1 << static_cast<uint8>(ECommonTeamComparison::SameTeam)) |
	(1 << static_cast<uint8>(ECommonTeamComparison::DifferentTeams)) |
	(1 << static_cast<uint8>(ECommonTeamComparison::NoTeam));
