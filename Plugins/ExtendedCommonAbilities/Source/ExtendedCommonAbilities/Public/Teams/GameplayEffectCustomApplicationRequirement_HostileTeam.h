// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "GameplayEffectCustomApplicationRequirement_HostileTeam.generated.h"


/**
 * Only apply this gameplay effect if the source has a hostile team attitude towards the target.
 */
UCLASS(meta = (DisplayName = "Hostile Team"))
class EXTENDEDCOMMONABILITIES_API UGameplayEffectCustomApplicationRequirement_HostileTeam : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()

public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
	                                                   const FGameplayEffectSpec& Spec,
	                                                   UAbilitySystemComponent* ASC) const override;
};
