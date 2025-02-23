// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedAbilityTagRelationshipMapping.h"


void UExtendedAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
                                                                            FGameplayTagContainer& OutTagsToBlock,
                                                                            FGameplayTagContainer& OutTagsToCancel) const
{
	for (const FExtendedAbilityTagRelationship& Relationship : Relationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			OutTagsToBlock.AppendTags(Relationship.BlockAbilitiesWithTag);
			OutTagsToCancel.AppendTags(Relationship.CancelAbilitiesWithTag);
		}
	}
}

void UExtendedAbilityTagRelationshipMapping::GetAbilityActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
                                                                                 FGameplayTagContainer& OutRequiredTags,
                                                                                 FGameplayTagContainer& OutBlockedTags) const
{
	for (const FExtendedAbilityTagRelationship& Relationship : Relationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			OutRequiredTags.AppendTags(Relationship.ActivationRequiredTags);
			OutBlockedTags.AppendTags(Relationship.ActivationBlockedTags);
		}
	}
}
