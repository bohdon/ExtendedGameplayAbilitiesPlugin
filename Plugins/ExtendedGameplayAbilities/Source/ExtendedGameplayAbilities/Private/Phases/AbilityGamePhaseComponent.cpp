// Copyright Bohdon Sayre, All Rights Reserved.


#include "Phases/AbilityGamePhaseComponent.h"

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Phases/GamePhaseAbility.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_GamePhase, "GamePhase")

DEFINE_LOG_CATEGORY(LogAbilityGamePhase);


UAbilityGamePhaseComponent::UAbilityGamePhaseComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UAbilityGamePhaseComponent::SetAbilitySystem(UAbilitySystemComponent* InAbilitySystem)
{
	AbilitySystem = InAbilitySystem;
}

void UAbilityGamePhaseComponent::StartPhase(TSubclassOf<UGamePhaseAbility> PhaseAbility)
{
	if (!ensure(AbilitySystem))
	{
		return;
	}

	// give and activate the ability
	FGameplayAbilitySpec AbilitySpec(PhaseAbility, 1, INDEX_NONE, this);
	const FGameplayAbilitySpecHandle SpecHandle = AbilitySystem->GiveAbilityAndActivateOnce(AbilitySpec);

	// check if its active, and store or trigger callbacks
	const FGameplayAbilitySpec* ActiveSpec = AbilitySystem->FindAbilitySpecFromHandle(SpecHandle);
	if (ActiveSpec && ActiveSpec->IsActive())
	{
		// add entry for end callbacks, phase tag will be set during OnBeginPhase
		FActiveGamePhaseEntry& Entry = ActivePhases.FindOrAdd(SpecHandle);
		// TODO
	}
	else
	{
		// trigger end callbacks immediately
		// TODO
	}
}

bool UAbilityGamePhaseComponent::IsPhaseActive(FGameplayTag PhaseTag)
{
	for (auto& Elem : ActivePhases)
	{
		const FActiveGamePhaseEntry& Entry = Elem.Value;
		if (Entry.PhaseTag.MatchesTag(PhaseTag))
		{
			return true;
		}
	}
	return false;
}

bool UAbilityGamePhaseComponent::IsAnyPhaseActive(FGameplayTagContainer PhaseTags)
{
	for (auto& Elem : ActivePhases)
	{
		const FActiveGamePhaseEntry& Entry = Elem.Value;
		if (Entry.PhaseTag.MatchesAny(PhaseTags))
		{
			return true;
		}
	}
	return false;
}

void UAbilityGamePhaseComponent::OnBeginPhase(const UGamePhaseAbility* Ability, const FGameplayAbilitySpecHandle AbilityHandle)
{
	const FGameplayTag NewPhaseTag = Ability->PhaseTag;

	UE_LOG(LogAbilityGamePhase, Log, TEXT("Beginning Game Phase: '%s' (%s)"), *NewPhaseTag.ToString(), *GetNameSafe(Ability));

	if (!ensure(AbilitySystem))
	{
		return;
	}

	// cancel any active phases that do  not match the new phase
	CancelAbilitiesForNewPhase(NewPhaseTag);

	// store the phase tag.
	// this is done after ending old phases so that IsPhaseActive
	// will not return true yet for the new phase while the old ones end.
	FActiveGamePhaseEntry& Entry = ActivePhases.FindOrAdd(AbilityHandle);
	Entry.PhaseTag = NewPhaseTag;

	// broadcast events
	// TODO
}

void UAbilityGamePhaseComponent::OnEndPhase(const UGamePhaseAbility* Ability, const FGameplayAbilitySpecHandle AbilityHandle)
{
	const FGameplayTag EndedPhaseTag = Ability->PhaseTag;

	UE_LOG(LogAbilityGamePhase, Log, TEXT("Ended Game Phase: '%s' (%s)"), *EndedPhaseTag.ToString(), *GetNameSafe(Ability));

	const FActiveGamePhaseEntry& Entry = ActivePhases.FindChecked(AbilityHandle);

	ActivePhases.Remove(AbilityHandle);

	// broadcast events
	// TODO
}

void UAbilityGamePhaseComponent::CancelAbilitiesForNewPhase(const FGameplayTag& NewPhaseTag)
{
	TArray<FGameplayAbilitySpec*> ActivePhasesSpecs;
	for (const auto& Elem : ActivePhases)
	{
		const FGameplayAbilitySpecHandle ActiveAbilityHandle = Elem.Key;
		if (FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(ActiveAbilityHandle))
		{
			ActivePhasesSpecs.Add(Spec);
		}
	}

	for (const FGameplayAbilitySpec* ActivePhaseSpec : ActivePhasesSpecs)
	{
		// get active phase tag from CDO
		const UGamePhaseAbility* ActiveAbility = Cast<UGamePhaseAbility>(ActivePhaseSpec->Ability);
		const FGameplayTag ActivePhaseTag = ActiveAbility->PhaseTag;

		// if the active phase does not match the new phase, cancel
		if (!NewPhaseTag.MatchesTag(ActivePhaseTag))
		{
			UE_LOG(LogAbilityGamePhase, Log, TEXT("Ending Game Phase: '%s' (%s)"), *ActivePhaseTag.ToString(), *GetNameSafe(ActiveAbility));

			AbilitySystem->CancelAbilityHandle(ActivePhaseSpec->Handle);
		}
	}
}
