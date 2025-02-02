// Copyright Bohdon Sayre, All Rights Reserved.


#include "AsyncAction_WaitForInitState.h"

#include "ExtendedCommonAbilitiesModule.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Actor.h"


UAsyncAction_WaitForInitState* UAsyncAction_WaitForInitState::WaitForInitState(UObject* Object, FGameplayTag RequiredState, bool bOnlyTriggerOnce)
{
	UAsyncAction_WaitForInitState* NewAction = NewObject<UAsyncAction_WaitForInitState>();
	NewAction->Object = Object;
	NewAction->RequiredState = RequiredState;
	NewAction->bOnlyTriggerOnce = bOnlyTriggerOnce;
	return NewAction;
}

void UAsyncAction_WaitForInitState::Activate()
{
	if (!Object.IsValid())
	{
		SetReadyToDestroy();
		return;
	}

	// find the target actor from the object
	AActor* Actor = nullptr;

	// determine if we are waiting for a specific feature,
	// this interface also provides a reliable way to retrieve the actor
	const IGameFrameworkInitStateInterface* InitInterface = Cast<IGameFrameworkInitStateInterface>(Object);
	const FName FeatureName = InitInterface ? InitInterface->GetFeatureName() : NAME_None;
	if (InitInterface)
	{
		Actor = InitInterface->GetOwningActor();
	}

	// try using the given object as an actor
	if (!Actor)
	{
		Actor = Cast<AActor>(Object.Get());
	}

	// try using the object as a component to find the actor
	if (!Actor)
	{
		if (const UActorComponent* Component = Cast<UActorComponent>(Object.Get()))
		{
			Actor = Component->GetOwner();
		}
	}

	if (!Actor)
	{
		UE_LOG(LogCommonAbilities, Error, TEXT("UAsyncAction_WaitForInitState: Object must be an Actor or ActorComponent: %s"), *Object->GetName());
		SetReadyToDestroy();
		return;
	}


	const UGameInstance* GameInstance = Actor->GetGameInstance();
	if (!GameInstance)
	{
		SetReadyToDestroy();
		return;
	}

	// start listening for init state change (or trigger the callback if already there)
	const FActorInitStateChangedDelegate ChangeDelegate = FActorInitStateChangedDelegate::CreateUObject(this, &ThisClass::OnActorInitStateChanged);

	UGameFrameworkComponentManager* ComponentManager = GameInstance->GetSubsystem<UGameFrameworkComponentManager>();
	ComponentManager->RegisterAndCallForActorInitState(Actor, FeatureName, RequiredState, ChangeDelegate);
}

void UAsyncAction_WaitForInitState::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	OnStateReached.Broadcast(Params);

	if (bOnlyTriggerOnce || !Object.IsValid() || HaveAllFeaturesReachedInitState(Params.OwningActor))
	{
		SetReadyToDestroy();
	}
}

bool UAsyncAction_WaitForInitState::HaveAllFeaturesReachedInitState(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	const UGameInstance* GameInstance = Actor->GetGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	const UGameFrameworkComponentManager* ComponentManager = GameInstance->GetSubsystem<UGameFrameworkComponentManager>();
	return ComponentManager->HaveAllFeaturesReachedInitState(Actor, RequiredState);
}
