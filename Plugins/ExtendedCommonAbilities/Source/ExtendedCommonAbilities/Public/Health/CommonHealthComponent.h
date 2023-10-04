// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "NativeGameplayTags.h"
#include "Components/ActorComponent.h"
#include "CommonHealthComponent.generated.h"

class UAbilitySystemComponent;


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Death);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dying);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dead);


UENUM(BlueprintType)
enum class ECommonHealthState : uint8
{
	Alive,
	DeathStarted,
	DeathFinished,
};


/**
 * Handles events related to life and death.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class EXTENDEDCOMMONABILITIES_API UCommonHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommonHealthComponent(const FObjectInitializer& ObjectInitializer);

	/** Automatically set the ability system by retrieving it from the owning actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	bool bAutoRegisterAbilitySystem;

	/**
	 * The attribute that represents the character's main health.
	 * The gameplay event 'Event.Death' is sent when this reaches 0.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	FGameplayAttribute HealthAttribute;

	/** The current state of health. */
	UPROPERTY(VisibleAnywhere, Replicated, ReplicatedUsing=OnRep_HealthState, Category = "Health")
	ECommonHealthState HealthState;

	UFUNCTION()
	void OnRep_HealthState(ECommonHealthState OldHealthState);

	UFUNCTION(BlueprintPure, Category = "CommonAbilities|Health")
	bool IsAlive() const { return HealthState <= ECommonHealthState::Alive; }

	/**
	 * Begin dying, called automatically when HP is depleted.
	 * Can be called when HP is not 0 to prematurely kill a character or object.
	 */
	UFUNCTION(BlueprintCallable, Category = "CommonAbilities|Health")
	void StartDeath();

	/**
	 * Finish dying, intended to be called from death abilities after animation or other fx.
	 * StartDeath must be called before this.
	 */
	UFUNCTION(BlueprintCallable, Category = "CommonAbilities|Health")
	void FinishDeath();

	/** Set the ability system to use for tracking health. */
	UFUNCTION(BlueprintCallable, Category = "CommonAbilities|Health")
	void SetAbilitySystem(UAbilitySystemComponent* InAbilitySystem);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthStateChangedDelegate, AActor*, OwningActor);

	UPROPERTY(BlueprintAssignable, DisplayName = "OnDeathStartedEvent")
	FHealthStateChangedDelegate OnDeathStartedEvent_BP;

	UPROPERTY(BlueprintAssignable, DisplayName = "OnDeathFinishedEvent")
	FHealthStateChangedDelegate OnDeathFinishedEvent_BP;

protected:
	/** Ability system being monitored. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	virtual void InitializeComponent() override;
	virtual void OnUnregister() override;

	void ClearGameplayTags();

	void OnHPChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when health state has changed to DeathStarted. */
	void OnDeathStarted();

	/** Called when health state has changed to DeathFinished. */
	void OnDeathFinished();

public:
	/** Return the CommonHealthComponent of an actor, if one exists. */
	UFUNCTION(BlueprintPure, Category = "CommonAbilities|Health")
	static UCommonHealthComponent* GetHealthComponent(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<UCommonHealthComponent>() : nullptr;
	}
};
