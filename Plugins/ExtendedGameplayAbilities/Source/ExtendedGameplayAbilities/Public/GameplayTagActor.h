// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayTagActor.generated.h"


/**
 * Base class for a simple actor that implements the IGameplayTagAssetInterface.
 * Useful for simple gameplay objects, spawn points
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API AGameplayTagActor : public AActor,
                                                        public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	/** The tags that this actor has. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer OwnedTags;

	// IGameplayTagAssetInterface
	FORCEINLINE virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer.Reset();
		TagContainer.AppendTags(OwnedTags);
	}

	FORCEINLINE virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override
	{
		return OwnedTags.HasTag(TagToCheck);
	}

	FORCEINLINE virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override
	{
		return OwnedTags.HasAll(TagContainer);
	}

	FORCEINLINE virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override
	{
		return OwnedTags.HasAny(TagContainer);
	}
};
