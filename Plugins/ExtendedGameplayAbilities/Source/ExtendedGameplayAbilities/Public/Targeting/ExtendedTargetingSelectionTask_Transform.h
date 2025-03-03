// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingTask.h"
#include "ExtendedTargetingSelectionTask_Transform.generated.h"


/**
 * Creates Transform targeting data from hit results in the default targeting data store,
 * with extensible logic for calculating the desired rotation.
 */
UCLASS(Blueprintable)
class EXTENDEDGAMEPLAYABILITIES_API UExtendedTargetingSelectionTask_Transform : public UTargetingTask
{
	GENERATED_BODY()

public:
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;

protected:
	/** If true, clear any hit results so that they're fully replaced by transforms. */
	UPROPERTY(EditAnywhere)
	bool bReplaceHitResults = true;

	/** The default rotation to use in GetTargetTransformForHitResult unless overridden. */
	UPROPERTY(EditAnywhere)
	FRotator DefaultRotation;

	/** Compute the transform to use for a hit result. */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Transform Selection")
	FTransform GetTargetTransformForHitResult(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& ResultData) const;
};
