// Copyright Bohdon Sayre, All Rights Reserved.


#include "AbilityPawn.h"

#include "AbilitiesInitStateComponent.h"
#include "AbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesModule.h"
#include "PawnAbilityInputComponent.h"


FName AAbilityPawn::PawnAbilityInputComponentName(TEXT("PawnAbilityInputComponent"));


AAbilityPawn::AAbilityPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(InitStateComponentName, UAbilitiesInitStateComponent::StaticClass()))
{
	PawnAbilityInputComponent = CreateDefaultSubobject<UPawnAbilityInputComponent>(PawnAbilityInputComponentName);
}

void AAbilityPawn::PostLoad()
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
