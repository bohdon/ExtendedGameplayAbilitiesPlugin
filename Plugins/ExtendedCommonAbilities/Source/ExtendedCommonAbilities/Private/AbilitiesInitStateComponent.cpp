// Copyright Bohdon Sayre, All Rights Reserved.


#include "AbilitiesInitStateComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "ExtendedCommonAbilitiesModule.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"


UAbilitiesInitStateComponent::UAbilitiesInitStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbilitiesInitStateComponent::InitializeAbilitySystem(UAbilitySystemComponent* InAbilitySystem, AActor* Owner)
{
	check(InAbilitySystem);
	check(Owner);

	if (AbilitySystem == InAbilitySystem)
	{
		// no change
		return;
	}

	if (AbilitySystem)
	{
		CleanupAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	const APawn* ExistingPawnAvatar = Cast<APawn>(InAbilitySystem->GetAvatarActor());

	if (ExistingPawnAvatar && ExistingPawnAvatar != Pawn)
	{
		// cleanup any leftover different pawn, which can happen
		// on clients if their new pawn is possessed before the old one is removed.
		ensure(!ExistingPawnAvatar->HasAuthority());

		if (UAbilitiesInitStateComponent* OtherPawnAbilityComp = ExistingPawnAvatar->FindComponentByClass<UAbilitiesInitStateComponent>())
		{
			OtherPawnAbilityComp->UninitializeAbilitySystem();
		}
	}

	UE_LOG(LogCommonAbilities, Verbose, TEXT("InitializeAbilitySystem %s, Owner: %s, Avatar: %s"),
	       *GetNameSafe(InAbilitySystem), *GetNameSafe(Owner), *GetNameSafe(Pawn));

	AbilitySystem = InAbilitySystem;
	AbilitySystem->InitAbilityActorInfo(Owner, Pawn);

	OnAbilitySystemInitializedEvent.Broadcast();
}

void UAbilitiesInitStateComponent::CleanupAbilitySystem()
{
	// cleanup ability system as long as we are still the avatar, otherwise someone else will take over
	if (AbilitySystem && AbilitySystem->GetAvatarActor() == GetOwner())
	{
		// cleanup previous ability system
		UninitializeAbilitySystem();
	}

	AbilitySystem = nullptr;
}

void UAbilitiesInitStateComponent::UninitializeAbilitySystem()
{
	check(AbilitySystem);
	check(AbilitySystem->GetAvatarActor() == GetOwner());

	AbilitySystem->RemoveAllGameplayCues();

	if (AbilitySystem->GetOwnerActor() == nullptr)
	{
		// if there's no owner, clear *all* actor info, not just the avatar
		AbilitySystem->ClearActorInfo();
	}
	else
	{
		AbilitySystem->SetAvatarActor(nullptr);
	}

	// only broadcast uninitialize when this pawn was the avatar and clean
	OnAbilitySystemUninitializedEvent.Broadcast();
}


void UAbilitiesInitStateComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		// transitioning to DataInitialized

		// find and initialize ability system
		UAbilitySystemComponent* NewAbilitySystem;
		AActor* NewOwner;
		FindAbilitySystemAndOwner(NewAbilitySystem, NewOwner);

		if (NewAbilitySystem && NewOwner)
		{
			InitializeAbilitySystem(NewAbilitySystem, NewOwner);
		}
	}
}

bool UAbilitiesInitStateComponent::CheckDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	// for ability systems to work, we also need a player state, and the matching controller.
	// wait for both here in addition to the super class requirements.

	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	// player state is required for retrieving the ability system
	if (!GetPlayerState<APlayerState>())
	{
		return false;
	}

	// if we're authority or autonomous, wait for a controller with registered ownership of the player state
	if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
	{
		const AController* Controller = GetController<AController>();

		const bool bHasControllerPairedWithPS = Controller && Controller->PlayerState && Controller->PlayerState->GetOwner() == Controller;
		if (!bHasControllerPairedWithPS)
		{
			return false;
		}
	}

	return Super::CheckDataAvailable(Manager);
}

void UAbilitiesInitStateComponent::FindAbilitySystemAndOwner(UAbilitySystemComponent*& OutAbilitySystem, AActor*& OutOwner)
{
	const APawn* Pawn = GetPawn<APawn>();
	OutOwner = GetPlayerState<APlayerState>();

	if (!ensure(Pawn && OutOwner))
	{
		return;
	}

	OutAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OutOwner);

	ensureAlwaysMsgf(OutAbilitySystem, TEXT("%s: no AbilitySystem found on PlayerState: %s"),
	                 *GetReadableName(), *OutOwner->GetName());
}
