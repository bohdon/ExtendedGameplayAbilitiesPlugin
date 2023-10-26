// Copyright Bohdon Sayre, All Rights Reserved.


#include "AI/BTTask_AbilityBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"


UBTTask_AbilityBase::UBTTask_AbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilitySystemComponent* UBTTask_AbilityBase::GetAbilitySystemComponent(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& InBlackboardKey) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	check(Blackboard);

	if (InBlackboardKey.SelectedKeyType != UBlackboardKeyType_Object::StaticClass())
	{
		UE_VLOG(OwnerComp.GetOwner(), LogAbilitySystem, Error,
		        TEXT("Blackboard key type must be set to 'Object', and represent an Actor or AbilitySystemComponent: %s"),
		        *InBlackboardKey.SelectedKeyName.ToString());
		return nullptr;
	}

	UObject* KeyObject = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	if (const AActor* Actor = Cast<AActor>(KeyObject))
	{
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
	if (UAbilitySystemComponent* AbilitySystem = Cast<UAbilitySystemComponent>(KeyObject))
	{
		return AbilitySystem;
	}
	return nullptr;
}
