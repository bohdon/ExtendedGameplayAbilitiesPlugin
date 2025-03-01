// Copyright Bohdon Sayre, All Rights Reserved.


#include "AbilityPlayerState.h"

#include "ExtendedAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Teams/CommonTeamStatics.h"


AAbilityPlayerState::AAbilityPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  TeamId(FGenericTeamId::NoTeam)
{
	AbilitySystem = CreateDefaultSubobject<UExtendedAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// replicate ability system at a high frequency
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AAbilityPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AAbilityPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (HasAuthority())
	{
		// note that any AIControllers should implement GetGenericTeamId to return this TeamId,
		// instead of the built-in AIController::TeamID
		TeamId = NewTeamId;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TeamId, this);
	}
}

FGenericTeamId AAbilityPlayerState::GetGenericTeamId() const
{
	return TeamId;
}

int32 AAbilityPlayerState::GetTeamId() const
{
	return UCommonTeamStatics::GenericTeamIdToInteger(TeamId);
}

void AAbilityPlayerState::OnRep_TeamId(FGenericTeamId OldTeamId)
{
}

void AAbilityPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TeamId, SharedParams);
}
