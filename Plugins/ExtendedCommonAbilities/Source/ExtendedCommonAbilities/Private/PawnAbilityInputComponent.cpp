// Copyright Bohdon Sayre, All Rights Reserved.


#include "PawnAbilityInputComponent.h"

#include "AbilitiesInitStateComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputComponent.h"
#include "ExtendedAbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesModule.h"
#include "GameFramework/PlayerController.h"
#include "Input/GameplayTagInputConfig.h"


UPawnAbilityInputComponent::UPawnAbilityInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UPawnAbilityInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// listen for ability system initialize
	if (UAbilitiesInitStateComponent* AbilityInitComp = GetOwner()->GetComponentByClass<UAbilitiesInitStateComponent>())
	{
		AbilityInitComp->OnAbilitySystemInitializedEvent.AddUObject(this, &ThisClass::OnAbilitySystemInitialized);
	}
}

void UPawnAbilityInputComponent::AddInputConfig(const UGameplayTagInputConfig* InputConfig)
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(GetOwner()->InputComponent))
	{
		BindAbilityActions(EnhancedInput, InputConfig);
	}
}

void UPawnAbilityInputComponent::RemoveInputConfig(const UGameplayTagInputConfig* InputConfig)
{
}

void UPawnAbilityInputComponent::OnAbilitySystemInitialized()
{
	if (GetController<APlayerController>())
	{
		AbilitySystem = Cast<UExtendedAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));

		if (GetOwner()->InputComponent)
		{
			SetupPlayerInput(GetOwner()->InputComponent);
		}
	}
}

void UPawnAbilityInputComponent::BindAbilityActions(UEnhancedInputComponent* EnhancedInput, const UGameplayTagInputConfig* InputConfig)
{
	for (const FGameplayTagInputAction& InputAction : InputConfig->InputActions)
	{
		if (!InputAction.InputAction || !InputAction.InputTag.IsValid())
		{
			continue;
		}

		EnhancedInput->BindAction(InputAction.InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnAbilityInputTagTriggered, InputAction.InputTag);
		EnhancedInput->BindAction(InputAction.InputAction, ETriggerEvent::Completed, this, &ThisClass::OnAbilityInputTagReleased, InputAction.InputTag);
	}
}

void UPawnAbilityInputComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (DefaultInputConfig)
		{
			BindAbilityActions(EnhancedInput, DefaultInputConfig);
		}
	}
}

void UPawnAbilityInputComponent::OnAbilityInputTagTriggered(const FInputActionInstance& InputActionInstance, FGameplayTag InputTag)
{
	if (bSendContinuousTriggers || !TriggeredInputTags.Contains(InputTag))
	{
		if (!bSendContinuousTriggers)
		{
			TriggeredInputTags.Add(InputTag);
		}

		OnAbilityInputTagPressed(InputTag);
	}
}

void UPawnAbilityInputComponent::OnAbilityInputTagPressed(FGameplayTag InputTag)
{
	UE_LOG(LogCommonAbilities, Verbose, TEXT("%s: InputAbilityTagPressed: %s"),
	       *GetName(), *InputTag.ToString());

	if (AbilitySystem)
	{
		AbilitySystem->AbilityTagInputPressed(InputTag);
	}
}

void UPawnAbilityInputComponent::OnAbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!bSendContinuousTriggers)
	{
		TriggeredInputTags.Remove(InputTag);
	}

	UE_LOG(LogCommonAbilities, Verbose, TEXT("%s: InputAbilityTagReleased: %s"),
	       *GetName(), *InputTag.ToString());

	if (AbilitySystem)
	{
		AbilitySystem->AbilityTagInputReleased(InputTag);
	}
}
