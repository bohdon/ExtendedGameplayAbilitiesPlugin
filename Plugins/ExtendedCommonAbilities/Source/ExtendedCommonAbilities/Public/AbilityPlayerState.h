// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "ModularPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "AbilityPlayerState.generated.h"

class UExtendedAbilitySystemComponent;


/**
 * PlayerState with an ability system component and support for teams.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API AAbilityPlayerState : public AModularPlayerState,
                                                        public IAbilitySystemInterface,
                                                        public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UExtendedAbilitySystemComponent> AbilitySystem;

public:
	AAbilityPlayerState(const FObjectInitializer& ObjectInitializer);

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "PlayerState")
	UExtendedAbilitySystemComponent* GetExtendedAbilitySystemComponent() const { return AbilitySystem; }

	// IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintPure, Category = "PlayerState")
	int32 GetTeamId() const;

protected:
	/** The team this player is on. */
	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	FGenericTeamId TeamId;

	UFUNCTION()
	virtual void OnRep_TeamId(FGenericTeamId OldTeamId);
};
