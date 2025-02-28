// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "PawnInitStateComponent.generated.h"

class UInputComponent;


/**
 * Coordinates the complex initialization process for pawns.
 * This is modeled after the ULyraPawnExtensionComponent and uses the same init state tags.
 * Designed for use with AInitStateCharacter, though you can implement calls to the Notify* functions
 * in your own pawn subclasses easily.
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class EXTENDEDCOMMONABILITIES_API UPawnInitStateComponent : public UPawnComponent,
                                                            public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UPawnInitStateComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * If true, a Controller is required before transitioning to the DataAvailable state.
	 * If Pawns may exist and be used without a controller set this to false.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitState")
	bool bWaitForController;

	/** The name of this actor feature. */
	static FName NAME_FeatureName;

	// IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_FeatureName; }
	virtual void CheckDefaultInitialization() override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

	/** Return true if all required data for this pawn is available, allowing a transition to the DataAvailable init state. */
	virtual bool CheckDataAvailable(UGameFrameworkComponentManager* Manager) const;

	/** Return true if all data is initialized, allowing a transition to DataInitialized init state. */
	virtual bool CheckDataInitialized(UGameFrameworkComponentManager* Manager) const;

	void NotifyControllerChanged();
	void NotifyPlayerStateReplicated();
	void NotifySetupPlayerInputComponent(UInputComponent* InputComponent);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnAnyPawnControllerChanged(APawn* Pawn, AController* Controller);

public:
	/** Return the pawn init state component from an actor. */
	UFUNCTION(BlueprintPure, Category = "InitState|Pawn")
	static UPawnInitStateComponent* GetPawnInitStateComponent(const AActor* Actor);
};
