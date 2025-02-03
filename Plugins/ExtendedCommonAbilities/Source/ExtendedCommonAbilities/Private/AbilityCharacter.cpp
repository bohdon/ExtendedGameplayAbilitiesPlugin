// Copyright Bohdon Sayre, All Rights Reserved.


#include "AbilityCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "ExtendedAbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesModule.h"
#include "Input/GameplayTagInputConfig.h"


AAbilityCharacter::AAbilityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(AInitStateCharacter::InitStateComponentName, UAbilitiesInitStateComponent::StaticClass()))
{
}

void AAbilityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		for (const UGameplayTagInputConfig* AbilityInputConfig : AbilityInputConfigs)
		{
			if (!AbilityInputConfig)
			{
				continue;
			}

			for (const FGameplayTagInputAction& InputAction : AbilityInputConfig->InputActions)
			{
				if (!InputAction.InputAction)
				{
					continue;
				}

				EnhancedInput->BindAction(InputAction.InputAction, ETriggerEvent::Triggered,
				                          this, &AAbilityCharacter::InputAbilityTagTriggered, InputAction.InputTag);
				EnhancedInput->BindAction(InputAction.InputAction, ETriggerEvent::Completed,
				                          this, &AAbilityCharacter::InputAbilityTagReleased, InputAction.InputTag);
			}
		}
	}
}

void AAbilityCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		AbilitySystem->GetOwnedGameplayTags(TagContainer);
	}
}

bool AAbilityCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

bool AAbilityCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AAbilityCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}

UAbilitySystemComponent* AAbilityCharacter::GetAbilitySystemComponent() const
{
	return GetAbilitiesInitStateComponent()->GetAbilitySystemComponent();
}

UAbilitiesInitStateComponent* AAbilityCharacter::GetAbilitiesInitStateComponent() const
{
	return Cast<UAbilitiesInitStateComponent>(GetInitStateComponent());
}

void AAbilityCharacter::InputAbilityTagTriggered(const FInputActionInstance& InputActionInstance, FGameplayTag InputTag)
{
	if (!TriggeredInputTags.Contains(InputTag))
	{
		TriggeredInputTags.Add(InputTag);

		InputAbilityTagPressed(InputTag);
	}
}

void AAbilityCharacter::InputAbilityTagPressed(FGameplayTag InputTag)
{
	if (IsMoveInputIgnored() && InputTag.MatchesAny(MovementAbilityInputTags))
	{
		UE_LOG(LogCommonAbilities, Verbose, TEXT("%s: Ignoring movement ability input: %s"),
		       *GetName(), *InputTag.ToString());
		return;
	}

	UE_LOG(LogCommonAbilities, Verbose, TEXT("%s: InputAbilityTagPressed: %s"),
	       *GetName(), *InputTag.ToString());
	if (UExtendedAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent<UExtendedAbilitySystemComponent>())
	{
		AbilitySystem->AbilityTagInputPressed(InputTag);
	}
}

void AAbilityCharacter::InputAbilityTagReleased(FGameplayTag InputTag)
{
	TriggeredInputTags.Remove(InputTag);

	UE_LOG(LogCommonAbilities, Verbose, TEXT("%s: InputAbilityTagReleased: %s"),
	       *GetName(), *InputTag.ToString());
	if (UExtendedAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent<UExtendedAbilitySystemComponent>())
	{
		AbilitySystem->AbilityTagInputReleased(InputTag);
	}
}

void AAbilityCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetAbilitiesInitStateComponent()->OnAbilitySystemInitializedEvent.AddUObject(this, &ThisClass::OnInitializeAbilitySystem);
	GetAbilitiesInitStateComponent()->OnAbilitySystemUninitializedEvent.AddUObject(this, &ThisClass::OnUninitializeAbilitySystem);
}

void AAbilityCharacter::OnInitializeAbilitySystem()
{
}

void AAbilityCharacter::OnUninitializeAbilitySystem()
{
}
