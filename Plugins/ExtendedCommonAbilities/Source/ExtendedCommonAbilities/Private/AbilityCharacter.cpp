// Copyright Bohdon Sayre, All Rights Reserved.


#include "AbilityCharacter.h"

#include "AbilitiesInitStateComponent.h"
#include "AbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesModule.h"
#include "PawnAbilityInputComponent.h"


FName AAbilityCharacter::PawnAbilityInputComponentName(TEXT("PawnAbilityInputComponent"));


AAbilityCharacter::AAbilityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(InitStateComponentName, UAbilitiesInitStateComponent::StaticClass()))
{
	PawnAbilityInputComponent = CreateDefaultSubobject<UPawnAbilityInputComponent>(PawnAbilityInputComponentName);
}

void AAbilityCharacter::PostLoad()
{
	Super::PostLoad();

	if (!AbilityInputConfigs_DEPRECATED.IsEmpty())
	{
		if (UPawnAbilityInputComponent* AbilityInputComp = GetPawnAbilityInputComponent())
		{
			if (!AbilityInputComp->DefaultInputConfig)
			{
				AbilityInputComp->DefaultInputConfig = AbilityInputConfigs_DEPRECATED[0];
			}
		}

		UE_CLOG(AbilityInputConfigs_DEPRECATED.Num() > 1, LogCommonAbilities, Warning,
		        TEXT("Multiple AbilityInputConfigs on AAbilityCharacter are deprecated, "
			        "add additional configs at runtime with UPawnAbilityInputComponent::AddInputConfig"));

		AbilityInputConfigs_DEPRECATED.Empty();
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

void AAbilityCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetAbilitiesInitStateComponent()->OnAbilitySystemInitializedEvent.AddUObject(this, &ThisClass::OnInitializeAbilitySystem);
	GetAbilitiesInitStateComponent()->OnAbilitySystemUninitializedEvent.AddUObject(this, &ThisClass::OnUninitializeAbilitySystem);
}

void AAbilityCharacter::OnInitializeAbilitySystem()
{
}

void AAbilityCharacter::OnUninitializeAbilitySystem()
{
}
