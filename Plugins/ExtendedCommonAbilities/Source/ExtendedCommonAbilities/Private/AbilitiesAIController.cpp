// Copyright Bohdon Sayre. All Rights Reserved.


#include "AbilitiesAIController.h"

#include "AbilityPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/PlayerState.h"


AAbilitiesAIController::AAbilitiesAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bWantsPlayerState = true;
}

UAbilitySystemComponent* AAbilitiesAIController::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState);
}

void AAbilitiesAIController::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		AbilitySystem->GetOwnedGameplayTags(TagContainer);
	}
	else
	{
		TagContainer.Reset();
	}
}

bool AAbilitiesAIController::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

bool AAbilitiesAIController::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AAbilitiesAIController::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}

FGenericTeamId AAbilitiesAIController::GetGenericTeamId() const
{
	// use the team ID of the ability player state (not AAIController::TeamID),
	// since the player state adds team change events
	const AAbilityPlayerState* AbilityPlayerState = GetPlayerState<AAbilityPlayerState>();
	return AbilityPlayerState ? AbilityPlayerState->GetGenericTeamId() : FGenericTeamId();
}

void AAbilitiesAIController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (AAbilityPlayerState* AbilityPlayerState = GetPlayerState<AAbilityPlayerState>())
	{
		AbilityPlayerState->SetGenericTeamId(NewTeamId);
	}
}

void AAbilitiesAIController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		
	}
}
