// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameplayTagInputConfig.generated.h"

class UInputAction;


/** Defines a single mapping between an enhanced input action and a gameplay tag. */
USTRUCT(BlueprintType)
struct FGameplayTagInputAction
{
	GENERATED_BODY()

	/** The action driving this input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputTag")
	TObjectPtr<UInputAction> InputAction = nullptr;

	/** The input tag to trigger from this input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputTag")
	FGameplayTag InputTag;
};


/**
 * Defines mappings between enhanced input actions and gameplay tags.
 */
UCLASS(BlueprintType, Const)
class EXTENDEDGAMEPLAYABILITIES_API UGameplayTagInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UGameplayTagInputConfig(const FObjectInitializer& ObjectInitializer);

	/** List of input actions mapped to gameplay tags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (TitleProperty = "{InputTag}"), Category = "InputTag")
	TArray<FGameplayTagInputAction> InputActions;

	/** Return an input action that is mapped to a gameplay tag. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "InputTag")
	const UInputAction* FindInputActionByTag(const FGameplayTag& InputTag) const;
};
