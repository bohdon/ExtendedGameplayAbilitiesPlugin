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
