// Copyright Bohdon Sayre, All Rights Reserved.


#include "Teams/TeamComparisonGameplayEffectComponent.h"

#include "AbilitySystemComponent.h"
#include "Teams/CommonTeamsComponent.h"
#include "Teams/CommonTeamStatics.h"
#include "UI/VM_ActiveGameplayEffects.h"


UTeamComparisonGameplayEffectComponent::UTeamComparisonGameplayEffectComponent()
{
	AttitudeMask = FCommonTeamTypes::AllAttitudesMask;
	ComparisonMask = FCommonTeamTypes::AllComparisonsMask;

#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Team Comparison to Apply This Effect");
#endif
}

bool UTeamComparisonGameplayEffectComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer,
                                                                    const FGameplayEffectSpec& GESpec) const
{
	const UCommonTeamsComponent* TeamsComp = UCommonTeamStatics::GetTeamsComponent(ActiveGEContainer.Owner);
	if (!TeamsComp)
	{
		// no teams setup, allow the effect
		return true;
	}

	const AActor* Instigator = GESpec.GetEffectContext().GetInstigator();
	if (!Instigator)
	{
		// require an instigator
		return false;
	}

	if (AttitudeMask != FCommonTeamTypes::AllAttitudesMask)
	{
		const ETeamAttitude::Type Attitude = TeamsComp->GetAttitude(Instigator, ActiveGEContainer.Owner->GetOwner());
		if (!FCommonTeamTypes::MatchesAttitudeMask(Attitude, AttitudeMask))
		{
			return false;
		}
	}

	if (ComparisonMask != FCommonTeamTypes::AllComparisonsMask)
	{
		const ECommonTeamComparison Comparison = TeamsComp->CompareTeams(Instigator, ActiveGEContainer.Owner->GetOwner());
		if (!FCommonTeamTypes::MatchesComparisonMask(Comparison, ComparisonMask))
		{
			return false;
		}
	}

	return true;
}
