// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityPawn.h"

#include "AbilitiesInitStateComponent.h"
#include "EnhancedInputComponent.h"
#include "ExtendedAbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesModule.h"
#include "Input/GameplayTagInputConfig.h"

AAbilityPawn::AAbilityPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(AInitStatePawn::InitStateComponentName, UAbilitiesInitStateComponent::StaticClass()))
{
}

void AAbilityPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
										  this, &ThisClass::InputAbilityTagTriggered, InputAction.InputTag);
				EnhancedInput->BindAction(InputAction.InputAction, ETriggerEvent::Completed,
										  this, &ThisClass::InputAbilityTagReleased, InputAction.InputTag);
			}
		}
	}
}

void AAbilityPawn::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		AbilitySystem->GetOwnedGameplayTags(TagContainer);
	}
}

bool AAbilityPawn::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

bool AAbilityPawn::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AAbilityPawn::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		return AbilitySystem->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}

UAbilitySystemComponent* AAbilityPawn::GetAbilitySystemComponent() const
{
	return GetAbilitiesInitStateComponent()->GetAbilitySystemComponent();
}

UAbilitiesInitStateComponent* AAbilityPawn::GetAbilitiesInitStateComponent() const
{
	return Cast<UAbilitiesInitStateComponent>(GetInitStateComponent());
}

void AAbilityPawn::InputAbilityTagTriggered(const FInputActionInstance& InputActionInstance, FGameplayTag InputTag)
{
	if (!TriggeredInputTags.Contains(InputTag))
	{
		TriggeredInputTags.Add(InputTag);

		InputAbilityTagPressed(InputTag);
	}
}

void AAbilityPawn::InputAbilityTagPressed(FGameplayTag InputTag)
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

void AAbilityPawn::InputAbilityTagReleased(FGameplayTag InputTag)
{
	TriggeredInputTags.Remove(InputTag);

	UE_LOG(LogCommonAbilities, Verbose, TEXT("%s: InputAbilityTagReleased: %s"),
		   *GetName(), *InputTag.ToString());
	if (UExtendedAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent<UExtendedAbilitySystemComponent>())
	{
		AbilitySystem->AbilityTagInputReleased(InputTag);
	}
}


void AAbilityPawn::BeginPlay()
{
	Super::BeginPlay();
	
	GetAbilitiesInitStateComponent()->OnAbilitySystemInitializedEvent.AddUObject(this, &ThisClass::OnInitializeAbilitySystem);
	GetAbilitiesInitStateComponent()->OnAbilitySystemUninitializedEvent.AddUObject(this, &ThisClass::OnUninitializeAbilitySystem);
}

void AAbilityPawn::OnInitializeAbilitySystem()
{
}

void AAbilityPawn::OnUninitializeAbilitySystem()
{
}
