// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingTask.h"
#include "ExtendedTargetingFilterTask_SingleResult.generated.h"


UENUM(BlueprintType)
enum class ETargetingFilterSingleResultType : uint8
{
	First,
	Last,
	Random,
};


/**
 * Selects a single result based on a heuristic.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UExtendedTargetingFilterTask_SingleResult : public UTargetingTask
{
	GENERATED_BODY()

protected:
	/** The type of result to select. */
	UPROPERTY(EditAnywhere, Category = "Filter")
	ETargetingFilterSingleResultType ResultType;

public:
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
};
