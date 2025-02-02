// Copyright Bohdon Sayre, All Rights Reserved.


#include "Animation/AnimNotifyState_GameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"


UAnimNotifyState_GameplayTag::UAnimNotifyState_GameplayTag()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(100, 255, 225);
#endif
}

FString UAnimNotifyState_GameplayTag::GetNotifyName_Implementation() const
{
	if (GameplayTag.IsValid())
	{
		return FString::Printf(TEXT("Tag: %s"), *GameplayTag.ToString());
	}

	return Super::GetNotifyName_Implementation();
}

void UAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                               const FAnimNotifyEventReference& EventReference)
{
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		AbilitySystem->AddLooseGameplayTag(GameplayTag);
	}
}

void UAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		AbilitySystem->RemoveLooseGameplayTag(GameplayTag);
	}
}
