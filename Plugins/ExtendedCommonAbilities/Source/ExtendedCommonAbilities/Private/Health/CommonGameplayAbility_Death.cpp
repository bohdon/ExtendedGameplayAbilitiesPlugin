// Copyright Bohdon Sayre, All Rights Reserved.


#include "Health/CommonGameplayAbility_Death.h"

#include "AbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesTags.h"
#include "Health/CommonHealthComponent.h"


UCommonGameplayAbility_Death::UCommonGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bStartDeathOnActivate(true)
{
	// it's possible that a character may die before the previous character finishes dying in some games,
	// ensure that each pawn fully finishes their dying process.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	bServerRespectsRemoteAbilityCancellation = false;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// setup trigger by gameplay event
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = ExtendedCommonAbilitiesTags::TAG_Event_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UCommonGameplayAbility_Death::StartDeath()
{
	HealthComponent = GetHealthComponent();
	if (HealthComponent.IsValid())
	{
		HealthComponent->StartDeath();
	}
}

void UCommonGameplayAbility_Death::FinishDeath()
{
	if (HealthComponent.IsValid())
	{
		HealthComponent->FinishDeath();
	}
}

void UCommonGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get();

	// cancel all abilities, except those that intentionally persist, and this one
	const FGameplayTagContainer CancelIgnoreTags(ExtendedCommonAbilitiesTags::TAG_Ability_Trait_PersistOnDeath);
	AbilitySystem->CancelAbilities(nullptr, &CancelIgnoreTags, this);

	SetCanBeCanceled(false);

	if (bStartDeathOnActivate)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCommonGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// finish death, does nothing if already called
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UCommonHealthComponent* UCommonGameplayAbility_Death::GetHealthComponent() const
{
	return UCommonHealthComponent::GetHealthComponent(GetAvatarActorFromActorInfo());
}
