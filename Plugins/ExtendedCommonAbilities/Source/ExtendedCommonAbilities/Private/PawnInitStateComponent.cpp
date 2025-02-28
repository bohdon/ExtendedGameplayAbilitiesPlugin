// Copyright Bohdon Sayre, All Rights Reserved.


#include "PawnInitStateComponent.h"

#include "ExtendedCommonAbilitiesTags.h"
#include "NativeGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Controller.h"


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
		ExtendedCommonAbilitiesTags::TAG_InitState_Spawned,
		ExtendedCommonAbilitiesTags::TAG_InitState_DataAvailable,
		ExtendedCommonAbilitiesTags::TAG_InitState_DataInitialized,
		ExtendedCommonAbilitiesTags::TAG_InitState_GameplayReady
	};

	// attempt to progress this component's init state
	ContinueInitStateChain(StateChain);
}

bool UPawnInitStateComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	if (!CurrentState.IsValid() && DesiredState == ExtendedCommonAbilitiesTags::TAG_InitState_Spawned)
	{
		// just need the pawn to exist
		return GetPawn<APawn>() != nullptr;
	}
	else if (CurrentState == ExtendedCommonAbilitiesTags::TAG_InitState_Spawned &&
		DesiredState == ExtendedCommonAbilitiesTags::TAG_InitState_DataAvailable)
	{
		return CheckDataAvailable(Manager);
	}
	else if (CurrentState == ExtendedCommonAbilitiesTags::TAG_InitState_DataAvailable &&
		DesiredState == ExtendedCommonAbilitiesTags::TAG_InitState_DataInitialized)
	{
		return CheckDataInitialized(Manager);
	}
	else if (CurrentState == ExtendedCommonAbilitiesTags::TAG_InitState_DataInitialized &&
		DesiredState == ExtendedCommonAbilitiesTags::TAG_InitState_GameplayReady)
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
		if (Params.FeatureState == ExtendedCommonAbilitiesTags::TAG_InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UPawnInitStateComponent::CheckDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	const bool bHasAuthority = Pawn->HasAuthority();
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	if (bWaitForController && (bHasAuthority || bIsLocallyControlled))
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
	return Manager->HaveAllFeaturesReachedInitState(Pawn, ExtendedCommonAbilitiesTags::TAG_InitState_DataAvailable);
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

	const bool bSuccess = TryToChangeInitState(ExtendedCommonAbilitiesTags::TAG_InitState_Spawned);
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

UPawnInitStateComponent* UPawnInitStateComponent::GetPawnInitStateComponent(const AActor* Actor)
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
