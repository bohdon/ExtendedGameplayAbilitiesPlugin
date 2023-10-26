// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AbilityBase.generated.h"

class UAbilitySystemComponent;


/**
 * Base class for a task that uses an ability system.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UBTTask_AbilityBase : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_AbilityBase(const FObjectInitializer& ObjectInitializer);

protected:
	/**
	 * Return an AbilitySystemComponent from a BlackboardKey.
	 * Supports either an Actor with an ability system / interface, or a UAbilitySystemComponent object itself.
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& InBlackboardKey) const;
};
