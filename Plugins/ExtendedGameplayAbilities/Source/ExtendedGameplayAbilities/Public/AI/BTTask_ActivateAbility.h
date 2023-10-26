// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ActivateAbility.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;


/**
 * Activate a gameplay ability and optionally wait for it to end.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UBTTask_ActivateAbility : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAbility(const FObjectInitializer& ObjectInitializer);

	/** The ability to activate. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass;

	/** Activate the first ability with all of these tags. Ignored when using AbilityClass. */
	UPROPERTY(EditAnywhere, Meta = (EditCondition = "!AbilityClass"), Category = "Ability")
	FGameplayTagContainer AbilityTags;

	/** Wait for the ability to end before finishing this task. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	bool bWaitForAbilityEnd;

	/** If the ability is already active, cancel and re-activate it. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	bool bRestartAbility;

	/** If true, fail this task if the ability is canceled instead of ending naturally. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	bool bFailOnAbilityCancel;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY()
	FGameplayAbilitySpecHandle SpecHandle;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemPtr;

	/** Return the ability system component from an object. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponentFromObject(UObject* Object) const;

	virtual FGameplayAbilitySpec* GetTargetAbilitySpec(const UAbilitySystemComponent& AbilitySystem) const;

	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp);

	static FGameplayAbilitySpec* GetFirstAbilityWithAllTags(const UAbilitySystemComponent& AbilitySystem, const FGameplayTagContainer& RequireTags);
};
