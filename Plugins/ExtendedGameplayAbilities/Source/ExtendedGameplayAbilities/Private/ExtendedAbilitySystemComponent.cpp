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

	// apply startup ability sets
	for (const UExtendedAbilitySet* AbilitySet : StartupAbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(this, this);
	}
}

void UExtendedAbilitySystemComponent::AbilityTagInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			Spec.InputPressed = true;
			if (Spec.IsActive())
			{
				if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
				{
					ServerSetInputPressed(Spec.Handle);
				}

				AbilitySpecInputPressed(Spec);

				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
			}
			else
			{
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

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			Spec.InputPressed = false;
			if (Spec.IsActive())
			{
				if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
				{
					ServerSetInputReleased(Spec.Handle);
				}

				AbilitySpecInputReleased(Spec);

				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
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
