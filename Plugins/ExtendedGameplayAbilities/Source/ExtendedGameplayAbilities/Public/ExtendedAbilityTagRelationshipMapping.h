// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ExtendedAbilityTagRelationshipMapping.generated.h"


/**
 * Defines additional cancel, block, and activation tag relationships for abilities by tag.
 */
USTRUCT(BlueprintType)
struct FExtendedAbilityTagRelationship
{
	GENERATED_BODY()

	/** The tag that an ability must have for the relationships to apply. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayTag AbilityTag;

	/** Abilities with these tags are cancelled when the ability is executed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	FGameplayTagContainer CancelAbilitiesWithTag;

	/** Abilities with these tags are blocked while the ability is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	FGameplayTagContainer BlockAbilitiesWithTag;

	/** The ability can only be activated if the activating actor/component has all of these tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	FGameplayTagContainer ActivationRequiredTags;

	/** The ability is blocked if the activating actor/component has any of these tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	FGameplayTagContainer ActivationBlockedTags;
};


/**
 * Mapping that defines how abilities block or cancel other abilities.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UExtendedAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The list of tag relationships to add to abilities by tag. */
	UPROPERTY(EditAnywhere, Category = "Ability", Meta = (TitleProperty = "AbilityTag"))
	TArray<FExtendedAbilityTagRelationship> Relationships;

	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
	                                    FGameplayTagContainer& OutTagsToBlock,
	                                    FGameplayTagContainer& OutTagsToCancel) const;

	void GetAbilityActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
	                                         FGameplayTagContainer& OutRequiredTags,
	                                         FGameplayTagContainer& OutBlockedTags) const;
};
