// Copyright Bohdon Sayre, All Rights Reserved.


#include "Phases/GamePhaseAbility.h"

#include "AbilitySystemComponent.h"
#include "Misc/DataValidation.h"
#include "Phases/AbilityGamePhaseComponent.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


#define LOCTEXT_NAMESPACE "UGamePhaseAbility"


UGamePhaseAbility::UGamePhaseAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
}

UAbilityGamePhaseComponent* UGamePhaseAbility::GetAbilityGamePhaseComponent(const FGameplayAbilitySpecHandle Handle,
                                                                            const FGameplayAbilityActorInfo* ActorInfo) const
{
	// game phase component will be set as the source object of this ability
	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);
	UAbilityGamePhaseComponent* PhaseComponent = Cast<UAbilityGamePhaseComponent>(SourceObject);

	ensureMsgf(PhaseComponent,
	           TEXT("UAbilityGamePhaseComponent was not set as the SourceObject of %s, "
		           "ensure game phase abilities are activated through the game phase component"),
	           *GetName());

	return PhaseComponent;
}

void UGamePhaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (UAbilityGamePhaseComponent* PhaseComponent = GetAbilityGamePhaseComponent(Handle, ActorInfo))
		{
			PhaseComponent->OnBeginPhase(this, Handle);
		}
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGamePhaseAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (UAbilityGamePhaseComponent* PhaseComponent = GetAbilityGamePhaseComponent(Handle, ActorInfo))
		{
			PhaseComponent->OnEndPhase(this, Handle);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

#if WITH_EDITOR
EDataValidationResult UGamePhaseAbility::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!PhaseTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(LOCTEXT("PhaseTagNotSet", "PhaseTag must be set to the tag representing this game phase"));
	}

	return Result;
}
#endif


#undef LOCTEXT_NAMESPACE
