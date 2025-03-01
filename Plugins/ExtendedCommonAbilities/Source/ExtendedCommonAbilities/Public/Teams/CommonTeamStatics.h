// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTeamTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonTeamStatics.generated.h"

class UCommonTeamsComponent;


/**
 * Static functions for working with teams and abilities.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API UCommonTeamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Return the teams component from the game state. */
	UFUNCTION(BlueprintPure, Category = "Teams", meta = (WorldContext = "WorldContextObject"))
	static UCommonTeamsComponent* GetTeamsComponent(const UObject* WorldContextObject);

	/** Compare the teams of two objects and return both their attitude and comparison result. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams")
	static void CompareTeams(const UObject* ObjectA, const UObject* ObjectB, TEnumAsByte<ETeamAttitude::Type>& Attitude, ECommonTeamComparison& Comparison);

	/** Convert a FGenericTeamId to an integer, returning INDEX_NONE if NoTeam. */
	FORCEINLINE static int32 GenericTeamIdToInteger(const FGenericTeamId& InTeamId)
	{
		return (InTeamId == FGenericTeamId::NoTeam) ? INDEX_NONE : static_cast<int32>(InTeamId);
	}

	/** Convert an integer to FGenericTeamId, returning NoTeam if < 0. */
	FORCEINLINE static FGenericTeamId IntegerToGenericTeamId(const int32 InTeamId)
	{
		return (InTeamId < 0) ? FGenericTeamId::NoTeam : FGenericTeamId(static_cast<uint8>(InTeamId));
	}
};
