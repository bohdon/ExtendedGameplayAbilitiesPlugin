// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_AbilityBase.h"
#include "BTTask_CancelAbility.generated.h"

class UGameplayAbility;


/**
 * Cancel an active GameplayAbility.
 * Supports canceling by class, tags, or both together.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UBTTask_CancelAbility : public UBTTask_AbilityBase
{
	GENERATED_BODY()

public:
	UBTTask_CancelAbility(const FObjectInitializer& ObjectInitializer);

	/** The ability to cancel. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass;

	/** Abilities must have all of these tags to be canceled. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer WithTags;

	/** Abilities must have none of these tags to be canceled. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer WithoutTags;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
