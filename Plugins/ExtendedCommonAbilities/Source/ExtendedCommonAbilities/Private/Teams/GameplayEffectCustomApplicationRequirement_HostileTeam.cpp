// Copyright Bohdon Sayre, All Rights Reserved.


#include "Teams/GameplayEffectCustomApplicationRequirement_HostileTeam.h"

#include "AbilitySystemComponent.h"
#include "Teams/CommonTeamsComponent.h"
#include "Teams/CommonTeamStatics.h"


bool UGameplayEffectCustomApplicationRequirement_HostileTeam::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
                                                                                                    const FGameplayEffectSpec& Spec,
                                                                                                    UAbilitySystemComponent* ASC) const
{
	if (const UCommonTeamsComponent* TeamsComp = UCommonTeamStatics::GetTeamsComponent(ASC))
	{
		if (const AActor* Instigator = Spec.GetEffectContext().GetInstigator())
		{
			return TeamsComp->GetAttitude(Instigator, ASC->GetOwner()) == ETeamAttitude::Hostile;
		}
	}
	return true;
}
