// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DynamicCooldownGameplayEffect.generated.h"


/**
 * A gameplay effect with a duration, used as the default effect for
 * dynamic cooldowns with UExtendedGameplayAbility.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UDynamicCooldownGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UDynamicCooldownGameplayEffect();
};
