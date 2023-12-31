﻿// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Looping.h"
#include "ExtendedGameplayCueNotify_Looping.generated.h"

/**
 * A AGameplayCueNotify_Looping with a bug fix that prevents OnRemove from properly being
 * called while GameplayCueNotifyTagCheckOnRemove is enabled (on by default).
 */
UCLASS(Blueprintable, NotPlaceable, Category = "GameplayCueNotify", Meta = (DisplayName = "Extended GCN Looping"))
class EXTENDEDGAMEPLAYABILITIES_API AExtendedGameplayCueNotify_Looping : public AGameplayCueNotify_Looping
{
	GENERATED_BODY()

public:
	AExtendedGameplayCueNotify_Looping();

	/** Return the gameplay cue tag for this notify actor. */
	UFUNCTION(BlueprintPure, Category = "GameplayCueNotify")
	FGameplayTag GetGameplayCueTag() const { return GameplayCueTag; }

	/** Return the active effects that grant this gameplay cue, if any. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameplayCueNotify")
	TArray<FActiveGameplayEffectHandle> GetGrantingActiveEffects() const;

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
};
