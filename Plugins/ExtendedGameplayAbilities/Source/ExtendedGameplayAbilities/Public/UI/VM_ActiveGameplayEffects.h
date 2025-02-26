﻿// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemViewModelBase.h"
#include "GameplayEffect.h"
#include "VM_ActiveGameplayEffects.generated.h"

class UGameplayEffectUIData;
class UVM_ActiveGameplayEffect;


/**
 * A view model representing multiple active gameplay effects from an ability system.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UVM_ActiveGameplayEffects : public UAbilitySystemViewModelBase
{
	GENERATED_BODY()

protected:
	/** Only include effects that match this query. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayEffectQuery EffectQuery;

	/** If set, only include effects that have matching a UI data component. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	TSubclassOf<UGameplayEffectUIData> RequiredUIDataClass;

public:
	UFUNCTION(BlueprintSetter)
	void SetEffectQuery(const FGameplayEffectQuery& NewQuery);

	UFUNCTION(BlueprintSetter)
	void SetRequiredUIDataClass(TSubclassOf<UGameplayEffectUIData> NewRequireUIDataClass);

	/** Return all active gameplay effects. */
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<FActiveGameplayEffectHandle> GetActiveEffects() const;

	/** Return a list of view models for all active gameplay effects. */
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<UVM_ActiveGameplayEffect*> GetActiveEffectViewModels() const;

protected:
	virtual void PreSystemChange() override;
	virtual void PostSystemChange() override;

	virtual void OnActiveGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent,
	                                         const FGameplayEffectSpec& GameplayEffectSpec,
	                                         FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	virtual void OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect);
};
