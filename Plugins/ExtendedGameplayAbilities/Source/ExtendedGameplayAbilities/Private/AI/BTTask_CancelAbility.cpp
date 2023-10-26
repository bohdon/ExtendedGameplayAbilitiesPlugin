// Copyright Bohdon Sayre, All Rights Reserved.


#include "AI/BTTask_CancelAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"


UBTTask_CancelAbility::UBTTask_CancelAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CancelAbility";
}

EBTNodeResult::Type UBTTask_CancelAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get ability system
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent(OwnerComp, BlackboardKey);
	if (!AbilitySystem)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogAbilitySystem, Error, TEXT("Could not get ability system from %s"), *BlackboardKey.SelectedKeyName.ToString());
		return EBTNodeResult::Failed;
	}

	if (AbilityClass)
	{
		AbilitySystem->CancelAbility(AbilityClass->GetDefaultObject<UGameplayAbility>());
	}

	if (!WithTags.IsEmpty() || !WithoutTags.IsEmpty())
	{
		AbilitySystem->CancelAbilities(&WithTags, &WithoutTags);
	}

	return EBTNodeResult::Succeeded;
}

FString UBTTask_CancelAbility::GetStaticDescription() const
{
	const bool bUsingClass = AbilityClass != nullptr;
	const bool bUsingTags = !WithTags.IsEmpty() || !WithoutTags.IsEmpty();

	const FString ClassDesc = FString::Printf(TEXT("\nof class %s"), *GetNameSafe(AbilityClass));

	const FString TagsDesc = FString::Printf(TEXT("\n%swith tags %s and without tags %s"),
	                                         bUsingClass ? TEXT("or ") : TEXT(""),
	                                         *WithTags.ToStringSimple(), *WithoutTags.ToStringSimple());

	// e.g. 'CancelAbility:\nof class MyAbility\nor with tags MyTag and without tags MyOtherTag'
	return FString::Printf(TEXT("%s:%s%s"), *Super::GetStaticDescription(), bUsingClass ? *ClassDesc : TEXT(""), bUsingTags ? *TagsDesc : TEXT(""));
}
