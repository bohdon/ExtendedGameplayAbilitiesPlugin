// Copyright Bohdon Sayre, All Rights Reserved.


#include "Targeting/ExtendedTargetingSelectionTask_Transform.h"

#include "Targeting/ExtendedTargetingSystemTypes.h"
#include "TargetingSystem/TargetingSubsystem.h"
#include "Types/TargetingSystemDataStores.h"


FTransform UExtendedTargetingSelectionTask_Transform::GetTargetTransformForHitResult_Implementation(const FTargetingRequestHandle& TargetingHandle,
                                                                                                    const FTargetingDefaultResultData& ResultData) const
{
	return FTransform(DefaultRotation, ResultData.HitResult.Location);
}

void UExtendedTargetingSelectionTask_Transform::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);

	FExtendedTargetingTransformResultsSet& TransformResults = FExtendedTargetingTransformResultsSet::FindOrAdd(TargetingHandle);

	if (FTargetingDefaultResultsSet* Results = FTargetingDefaultResultsSet::Find(TargetingHandle))
	{
		for (const FTargetingDefaultResultData& Result : Results->TargetResults)
		{
			FExtendedTargetingTransformResultData& TransformResult = TransformResults.TargetResults.Emplace_GetRef();
			TransformResult.Transform = GetTargetTransformForHitResult(TargetingHandle, Result);

#if ENABLE_DRAW_DEBUG
			if (UTargetingSubsystem::IsTargetingDebugEnabled())
			{
				if (UWorld* World = GetSourceContextWorld(TargetingHandle))
				{
					DrawDebugCoordinateSystem(World, TransformResult.Transform.GetLocation(), TransformResult.Transform.Rotator(), 50.f);
				}
			}
#endif
		}

		if (bReplaceHitResults)
		{
			Results->TargetResults.Reset();
		}
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}
