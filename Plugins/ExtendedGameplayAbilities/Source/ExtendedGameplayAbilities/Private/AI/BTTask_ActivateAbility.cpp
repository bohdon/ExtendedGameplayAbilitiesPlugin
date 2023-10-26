// Copyright Bohdon Sayre, All Rights Reserved.


#include "AI/BTTask_ActivateAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"

UBTTask_ActivateAbility::UBTTask_ActivateAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bWaitForAbilityEnd(true),
	  bRestartAbility(false),
	  bFailOnAbilityCancel(true)
{
	NodeName = "ActivateAbility";
	// required for ability end events to be bound
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get ability system
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent(OwnerComp, BlackboardKey);
	if (!AbilitySystem)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogAbilitySystem, Error, TEXT("Could not get ability system from %s"), *BlackboardKey.SelectedKeyName.ToString());
		return EBTNodeResult::Failed;
	}

	// find ability to activate
	FGameplayAbilitySpec* AbilitySpec = GetTargetAbilitySpec(*AbilitySystem);
	if (!AbilitySpec)
	{
		UE_CVLOG(AbilityClass, OwnerComp.GetOwner(), LogAbilitySystem, Error, TEXT("%s does not have ability %s"),
		         *GetNameSafe(AbilitySystem->GetOwner()), *AbilityClass->GetName());
		UE_CVLOG(!AbilityClass, OwnerComp.GetOwner(), LogAbilitySystem, Error, TEXT("%s does not have ability matching tags %s"),
		         *GetNameSafe(AbilitySystem->GetOwner()), *AbilityTags.ToString());
		return EBTNodeResult::Failed;
	}

	// cancel the ability if requested
	if (bRestartAbility && AbilitySpec->IsActive())
	{
		AbilitySystem->CancelAbilityHandle(AbilitySpec->Handle);
	}

	// bind events before activation, in case it ends immediately
	if (bWaitForAbilityEnd)
	{
		// TODO: use bt node memory
		SpecHandle = AbilitySpec->Handle;
		AbilitySystemPtr = AbilitySystem;
		AbilitySystem->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded, &OwnerComp);
	}

	const bool bSuccess = AbilitySystem->TryActivateAbility(AbilitySpec->Handle, true);
	if (!bSuccess)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogAbilitySystem, Error, TEXT("Failed to activate ability: %s"), *AbilitySpec->GetDebugString());
		return EBTNodeResult::Failed;
	}

	if (bWaitForAbilityEnd)
	{
		return EBTNodeResult::InProgress;
	}

	// TODO: what if it was activated then immediately canceled? this should respect bFailOnAbilityCancel
	// ability was activated, that's all that matters
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_ActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (bWaitForAbilityEnd && SpecHandle.IsValid() && AbilitySystemPtr.IsValid())
	{
		AbilitySystemPtr->CancelAbilityHandle(SpecHandle);
	}

	return EBTNodeResult::Aborted;
}

void UBTTask_ActivateAbility::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	SpecHandle = FGameplayAbilitySpecHandle();
	AbilitySystemPtr.Reset();
}

FString UBTTask_ActivateAbility::GetStaticDescription() const
{
	const FString AbilityDesc = AbilityClass || AbilityTags.IsEmpty()
		                            ? *GetNameSafe(AbilityClass)
		                            : FString::Printf(TEXT("with tags %s"), *AbilityTags.ToStringSimple());
	const FString ResultDesc = bWaitForAbilityEnd ? TEXT("\nWait for ability end") : TEXT("");
	// e.g. 'ActivateAbility: MyAbility on SelfActor, wait for ability end'
	return FString::Printf(TEXT("%s: %s on %s%s"),
	                       *Super::GetStaticDescription(), *AbilityDesc, *BlackboardKey.SelectedKeyName.ToString(), *ResultDesc);
}

void UBTTask_ActivateAbility::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp)
{
	if (!IsValid(OwnerComp))
	{
		return;
	}

	if (bWaitForAbilityEnd && SpecHandle.IsValid() && AbilityEndedData.AbilitySpecHandle == SpecHandle)
	{
		UE_VLOG(OwnerComp->GetOwner(), LogAbilitySystem, Log, TEXT("Ability %s: %s"),
		        AbilityEndedData.bWasCancelled ? TEXT("canceled") : TEXT("ended"), *GetNameSafe(AbilityEndedData.AbilityThatEnded));

		const EBTNodeResult::Type CancelResult = bFailOnAbilityCancel ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
		FinishLatentTask(*OwnerComp, AbilityEndedData.bWasCancelled ? CancelResult : EBTNodeResult::Succeeded);
	}
}

FGameplayAbilitySpec* UBTTask_ActivateAbility::GetTargetAbilitySpec(const UAbilitySystemComponent& AbilitySystem) const
{
	if (AbilityClass)
	{
		return AbilitySystem.FindAbilitySpecFromClass(AbilityClass);
	}
	if (!AbilityTags.IsEmpty())
	{
		return GetFirstAbilityWithAllTags(AbilitySystem, AbilityTags);
	}
	return nullptr;
}

FGameplayAbilitySpec* UBTTask_ActivateAbility::GetFirstAbilityWithAllTags(const UAbilitySystemComponent& AbilitySystem,
                                                                          const FGameplayTagContainer& RequireTags)
{
	TArray<FGameplayAbilitySpec*> MatchingAbilities;
	AbilitySystem.GetActivatableGameplayAbilitySpecsByAllMatchingTags(RequireTags, MatchingAbilities);
	return !MatchingAbilities.IsEmpty() ? MatchingAbilities[0] : nullptr;
}
