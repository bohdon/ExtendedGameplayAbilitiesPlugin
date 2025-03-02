// Copyright Bohdon Sayre, All Rights Reserved.


#include "Targeting/ExtendedTargetingFilterTask_SingleResult.h"


void UExtendedTargetingFilterTask_SingleResult::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);

	if (TargetingHandle.IsValid())
	{
		if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
		{
			const int32 NumTargets = ResultData->TargetResults.Num();
			if (NumTargets > 0)
			{
				switch (ResultType)
				{
				case ETargetingFilterSingleResultType::First:
					ResultData->TargetResults = {ResultData->TargetResults[0]};
					break;
				case ETargetingFilterSingleResultType::Last:
					ResultData->TargetResults = {ResultData->TargetResults.Last()};
					break;
				case ETargetingFilterSingleResultType::Random:
					ResultData->TargetResults = {ResultData->TargetResults[FMath::RandHelper(ResultData->TargetResults.Num())]};
					break;
				}
			}
		}
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}
