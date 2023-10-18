// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_WaitForInitState.generated.h"


UCLASS()
class EXTENDEDCOMMONABILITIES_API UAsyncAction_WaitForInitState : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Wait for an actor or actor feature to reach an init state via the GameFrameworkComponentManager.
	 * If the given object implements the IGameFrameworkInitStateInterface, it will be used as the specific
	 * feature to wait for, otherwise OnStateReached will be called for the first actor feature to reach the target state.
	 * @param Object The object whose init state is being monitored.
	 * @param RequiredState The target init state to wait for
	 * @param bOnlyTriggerOnce If true, destroy the async action after the first feature reaches the target init state, otherwise wait for all features.
	 */
	UFUNCTION(BlueprintCallable, Meta = (BlueprintInternalUseOnly = true), Category = "InitState")
	static UAsyncAction_WaitForInitState* WaitForInitState(UObject* Object, FGameplayTag RequiredState, bool bOnlyTriggerOnce = true);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateReachedDynDelegate, const FActorInitStateChangedParams&, Params);

	/** Called when the init state is reached, or immediately if already in that state. */
	UPROPERTY(BlueprintAssignable)
	FOnStateReachedDynDelegate OnStateReached;

	virtual void Activate() override;

protected:
	/** The object whose init state is being monitored. */
	TWeakObjectPtr<UObject> Object;

	/** The target init state to wait for. */
	FGameplayTag RequiredState;

	/** If true, destroy the async action after the first feature reaches the target init state, otherwise wait for all features. */
	bool bOnlyTriggerOnce;

	void OnActorInitStateChanged(const FActorInitStateChangedParams& Params);

	bool HaveAllFeaturesReachedInitState(AActor* Actor) const;
};
