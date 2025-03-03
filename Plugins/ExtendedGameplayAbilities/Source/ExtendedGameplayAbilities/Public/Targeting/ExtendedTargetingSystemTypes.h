// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/TargetingSystemDataStores.h"
#include "ExtendedTargetingSystemTypes.generated.h"


/**
 * Targeting result data that represents a simple Transform.
 */
USTRUCT(BlueprintType)
struct FExtendedTargetingTransformResultData
{
	GENERATED_BODY()

	FExtendedTargetingTransformResultData()
	{
	}

	/** The transform of this target */
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FTransform Transform;

	/** The score associated with this target */
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	float Score = 0.f;
};


/**
 * Custom targeting results data that contains an array of transforms.
 */
USTRUCT()
struct FExtendedTargetingTransformResultsSet
{
	GENERATED_BODY()

public:
	FExtendedTargetingTransformResultsSet()
	{
	}

	EXTENDEDGAMEPLAYABILITIES_API static FExtendedTargetingTransformResultsSet& FindOrAdd(FTargetingRequestHandle Handle);
	EXTENDEDGAMEPLAYABILITIES_API static FExtendedTargetingTransformResultsSet* Find(FTargetingRequestHandle Handle);

	/** The transform results for this target */
	UPROPERTY()
	TArray<FExtendedTargetingTransformResultData> TargetResults;
};


DECLARE_TARGETING_DATA_STORE(FExtendedTargetingTransformResultsSet)
