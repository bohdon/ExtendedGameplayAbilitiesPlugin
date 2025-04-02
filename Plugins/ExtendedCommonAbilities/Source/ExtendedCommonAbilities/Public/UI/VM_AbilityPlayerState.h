// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VM_AbilityPlayerState.generated.h"


class AAbilityPlayerState;

/**
 * View model for an AbilityPlayerState, mostly to track their team assignment and changes.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API UVM_AbilityPlayerState : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	/** The owning player state. */
	UPROPERTY()
	TWeakObjectPtr<AAbilityPlayerState> PlayerState;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetPlayerState(AAbilityPlayerState* NewPlayerState);

	UFUNCTION(BlueprintPure, FieldNotify)
	AAbilityPlayerState* GetPlayerState() const { return PlayerState.Get(); }

	UFUNCTION(BlueprintPure, FieldNotify)
	int32 GetTeamId() const;

protected:
	void OnTeamChanged(UObject* TeamAgent, int32 NewTeamId, int32 OldTeamId);
};
