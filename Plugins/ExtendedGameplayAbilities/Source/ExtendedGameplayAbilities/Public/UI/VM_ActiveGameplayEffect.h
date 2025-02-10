﻿// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"
#include "MVVMViewModelBase.h"

#include "VM_ActiveGameplayEffect.generated.h"


/**
 * A viewmodel that contains a FActiveGameplayEffectHandle, and handles binding to
 * active effect changes.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UVM_ActiveGameplayEffect : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	/** The active gameplay effect handle. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FActiveGameplayEffectHandle ActiveEffectHandle;

	/** Info about the effect after it was removed. */
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FGameplayEffectRemovalInfo RemovalInfo;

public:
	UFUNCTION(BlueprintSetter)
	void SetActiveEffectHandle(FActiveGameplayEffectHandle NewHandle);

	FActiveGameplayEffectHandle GetActiveEffectHandle() const { return ActiveEffectHandle; }

	UFUNCTION(BlueprintPure, FieldNotify)
	int32 GetStackCount() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetStartTime() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetEndTime() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetDuration() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	bool IsInhibited() const;


	const FActiveGameplayEffect* GetActiveGameplayEffect() const;

protected:
	void OnEffectRemoved(const FGameplayEffectRemovalInfo& InRemovalInfo);

	void OnEffectStackChanged(FActiveGameplayEffectHandle Handle, int32 NewStackCount, int32 OldStackCount);

	void OnEffectTimeChanged(FActiveGameplayEffectHandle Handle, float NewStartTime, float NewDuration);

	void OnEffectInhibitionChanged(FActiveGameplayEffectHandle Handle, bool bNewIsInhibited);
};
