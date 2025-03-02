// Copyright Bohdon Sayre, All Rights Reserved.


#include "Teams/TargetingFilterTask_TeamComparison.h"

#include "Teams/CommonTeamsComponent.h"
#include "Teams/CommonTeamStatics.h"


UTargetingFilterTask_TeamComparison::UTargetingFilterTask_TeamComparison(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Attitude =
		(1 << ETeamAttitude::Friendly) |
		(1 << ETeamAttitude::Neutral) |
		(1 << ETeamAttitude::Hostile);
	Comparison =
		(1 << static_cast<uint8>(ECommonTeamComparison::SameTeam)) |
		(1 << static_cast<uint8>(ECommonTeamComparison::DifferentTeams)) |
		(1 << static_cast<uint8>(ECommonTeamComparison::NoTeam));
}

void UTargetingFilterTask_TeamComparison::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	// can't use the parent implementation, because it affects the target results order.
	// note that debug display of filtered items is not implemented here.

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);

	if (TargetingHandle.IsValid())
	{
		if (FTargetingDefaultResultsSet* ResultData = FTargetingDefaultResultsSet::Find(TargetingHandle))
		{
			const int32 NumTargets = ResultData->TargetResults.Num();
			for (int32 TargetIterator = NumTargets - 1; TargetIterator >= 0; --TargetIterator)
			{
				const FTargetingDefaultResultData& TargetResult = ResultData->TargetResults[TargetIterator];
				if (ShouldFilterTarget(TargetingHandle, TargetResult))
				{
					ResultData->TargetResults.RemoveAt(TargetIterator, EAllowShrinking::No);
				}
			}
		}
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

bool UTargetingFilterTask_TeamComparison::ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle,
                                                             const FTargetingDefaultResultData& TargetData) const
{
	static uint8 AllAttitudesFlag =
		(1 << ETeamAttitude::Friendly) |
		(1 << ETeamAttitude::Neutral) |
		(1 << ETeamAttitude::Hostile);
	static uint8 AllComparisonsFlag =
		(1 << static_cast<uint8>(ECommonTeamComparison::SameTeam)) |
		(1 << static_cast<uint8>(ECommonTeamComparison::DifferentTeams)) |
		(1 << static_cast<uint8>(ECommonTeamComparison::NoTeam));

	const AActor* HitActor = TargetData.HitResult.GetActor();
	if (!HitActor)
	{
		// require an actor to pass this filter
		return !bIncludeNonBlockingHit;
	}

	const UCommonTeamsComponent* TeamsComp = UCommonTeamStatics::GetTeamsComponent(HitActor);
	if (!TeamsComp)
	{
		// can't do anything without teams component, assume passed
		return false;
	}

	const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle);
	if (!SourceContext->InstigatorActor)
	{
		// require an instigator
		return true;
	}

	AActor* Instigator = SourceContext->InstigatorActor.Get();

	if (Attitude != AllAttitudesFlag)
	{
		const ETeamAttitude::Type HitAttitude = TeamsComp->GetAttitude(Instigator, HitActor);
		if (!((1 << HitAttitude) & Attitude))
		{
			return true;
		}
	}


	if (Comparison != AllComparisonsFlag)
	{
		const ECommonTeamComparison HitComparison = TeamsComp->CompareTeams(Instigator, HitActor);
		if (!((1 << static_cast<uint8>(HitComparison)) & Comparison))
		{
			return true;
		}
	}

	return false;
}
