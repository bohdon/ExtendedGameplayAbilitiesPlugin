// Copyright Bohdon Sayre, All Rights Reserved.

#include "ExtendedAbilitySystemComponent.h"

#include "ExtendedAbilitySet.h"


UExtendedAbilitySystemComponent::UExtendedAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectSpecSet UExtendedAbilitySystemComponent::MakeEffectSpecSet(const FGameplayEffectSet& EffectSet, float Level)
{
	FGameplayEffectSpecSet SpecSet;
	for (const TSubclassOf<UGameplayEffect> GameplayEffect : EffectSet.Effects)
	{
		FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		if (Spec.IsValid())
		{
			// assign set-by-caller magnitudes
			for (const auto& Item : EffectSet.SetByCallerMagnitudes)
			{
				const float Value = Item.Value.GetValueAtLevel(Level);
				Spec.Data->SetByCallerTagMagnitudes.Add(Item.Key, Value);
			}

			SpecSet.EffectSpecs.Add(Spec);
		}
	}
	return SpecSet;
}

TArray<FActiveGameplayEffectHandle> UExtendedAbilitySystemComponent::ApplyGameplayEffectSpecSetToSelf(const FGameplayEffectSpecSet& EffectSpecSet)
{
	TArray<FActiveGameplayEffectHandle> Result;
	for (const FGameplayEffectSpecHandle& SpecHandle : EffectSpecSet.EffectSpecs)
	{
		FActiveGameplayEffectHandle NewHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), GetPredictionKeyForNewAction());
		if (NewHandle.IsValid())
		{
			Result.Add(NewHandle);
		}
	}
	return Result;
}

void UExtendedAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// apply default tags
	for (const FGameplayTag& DefaultTag : DefaultTags)
	{
		SetLooseGameplayTagCount(DefaultTag, 1);
		SetReplicatedLooseGameplayTagCount(DefaultTag, 1);
	}

	// apply startup ability sets
	for (const UExtendedAbilitySet* AbilitySet : StartupAbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(this, this);
	}
}

void UExtendedAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	if (AbilitySpec.Ability)
	{
		OnGiveAbilityEvent.Broadcast(AbilitySpec);
	}
}

void UExtendedAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	if (AbilitySpec.Ability)
	{
		OnRemoveAbilityEvent.Broadcast(AbilitySpec);
	}
}

void UExtendedAbilitySystemComponent::AbilityTagInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	// loosely based on UAbilitySystemComponent::AbilityLocalInputPressed,
	// but without handling bReplicateInputDirectly (as it's not recommended)

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (Spec.IsActive())
			{
				AbilitySpecInputPressed(Spec);

				TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				for (UGameplayAbility* Instance : Instances)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
					                      Instance->GetCurrentActivationInfoRef().GetActivationPredictionKey());
				}
			}
			else
			{
				// Ability is not active, so try to activate it
				Spec.InputPressed = true;
				TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UExtendedAbilitySystemComponent::AbilityTagInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	// loosely based on UAbilitySystemComponent::AbilityLocalInputReleased,
	// but without handling bReplicateInputDirectly (as it's not recommended)

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (Spec.IsActive())
			{
				AbilitySpecInputReleased(Spec);

				TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				for (UGameplayAbility* Instance : Instances)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,
					                      Instance->GetCurrentActivationInfoRef().GetActivationPredictionKey());
				}
			}
		}
	}
}

void UExtendedAbilitySystemComponent::PressInputTag(const FGameplayTag& InputTag)
{
	AbilityTagInputPressed(InputTag);
}

void UExtendedAbilitySystemComponent::ReleaseInputTag(const FGameplayTag& InputTag)
{
	AbilityTagInputReleased(InputTag);
}
