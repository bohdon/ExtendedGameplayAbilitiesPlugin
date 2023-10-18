// Copyright Bohdon Sayre, All Rights Reserved.


#include "PawnInitStateComponent.h"

#include "NativeGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"


UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_Spawned, "InitState.Spawned",
                               "1: Actor/component has initially spawned and can be extended");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_DataAvailable, "InitState.DataAvailable",
                               "2: All required data has been loaded/replicated and is ready for initialization");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_DataInitialized, "InitState.DataInitialized",
                               "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_GameplayReady, "InitState.GameplayReady",
                               "4: The actor/component is fully ready for active gameplay");


FName UPawnInitStateComponent::NAME_FeatureName(TEXT("PawnInitState"));


UPawnInitStateComponent::UPawnInitStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bWaitForController(true)
{
}

void UPawnInitStateComponent::CheckDefaultInitialization()
{
	// before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = {
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	// attempt to progress this component's init state
	ContinueInitStateChain(StateChain);
}

bool UPawnInitStateComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		// just need the pawn to exist
		return GetPawn<APawn>() != nullptr;
	}
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		return CheckDataAvailable(Manager);
	}
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		return CheckDataInitialized(Manager);
	}
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UPawnInitStateComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// if another feature is now in DataAvailable, check if we can progress state
	if (Params.FeatureName != NAME_FeatureName)
	{
		if (Params.FeatureState == TAG_InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UPawnInitStateComponent::CheckDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	// if the pawn is locally controlled, ensure that controller exists or has been replicated
	if ((bWaitForController && Pawn->HasAuthority()) ||
		Pawn->IsPlayerControlled() || Pawn->IsBotControlled())
	{
		if (!GetController<AController>())
		{
			return false;
		}
	}

	return true;
}

bool UPawnInitStateComponent::CheckDataInitialized(UGameFrameworkComponentManager* Manager) const
{
	APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	// wait for any other features to reach DataAvailable state
	return Manager->HaveAllFeaturesReachedInitState(Pawn, TAG_InitState_DataAvailable);
}

void UPawnInitStateComponent::OnRegister()
{
	Super::OnRegister();

	RegisterInitStateFeature();
}

void UPawnInitStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// listen for pawn controller changes
	GetGameInstance<UGameInstance>()->GetOnPawnControllerChanged().AddUniqueDynamic(this, &UPawnInitStateComponent::OnAnyPawnControllerChanged);

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	const bool bSuccess = TryToChangeInitState(TAG_InitState_Spawned);
	ensureMsgf(bSuccess, TEXT("PawnInitStateComponent on %s failed to transition to Spawned init state."), *GetNameSafe(GetOwner()));

	CheckDefaultInitialization();
}

void UPawnInitStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance<UGameInstance>()->GetOnPawnControllerChanged().RemoveAll(this);

	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UPawnInitStateComponent::OnAnyPawnControllerChanged(APawn* Pawn, AController* Controller)
{
	if (Pawn == GetPawn<APawn>())
	{
		CheckDefaultInitialization();
	}
}

UPawnInitStateComponent* UPawnInitStateComponent::FindPawnInitStateComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UPawnInitStateComponent>() : nullptr;
}

void UPawnInitStateComponent::NotifyControllerChanged()
{
	CheckDefaultInitialization();
}

void UPawnInitStateComponent::NotifyPlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UPawnInitStateComponent::NotifySetupPlayerInputComponent(UInputComponent* InputComponent)
{
	CheckDefaultInitialization();
}
