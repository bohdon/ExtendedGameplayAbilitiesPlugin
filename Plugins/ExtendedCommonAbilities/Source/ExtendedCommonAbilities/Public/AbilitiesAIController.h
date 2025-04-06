// Copyright Bohdon Sayre. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitiesAIController.generated.h"


/**
 * An AIController designed for use with an ability system on the player state.
 * Implements the ability system and gameplay tag interfaces.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API AAbilitiesAIController : public AAIController,
                                                           public IAbilitySystemInterface,
                                                           public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AAbilitiesAIController();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
};
