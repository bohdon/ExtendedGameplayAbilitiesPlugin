﻿// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_ActiveGameplayEffects.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectUIData.h"
#include "UI/VM_ActiveGameplayEffect.h"


void UVM_ActiveGameplayEffects::SetEffectQuery(const FGameplayEffectQuery& NewQuery)
{
	PreSystemChange();
	EffectQuery = NewQuery;
	PostSystemChange();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(EffectQuery);
}

void UVM_ActiveGameplayEffects::SetRequiredUIDataClass(TSubclassOf<UGameplayEffectUIData> NewRequireUIDataClass)
{
	if (RequiredUIDataClass != NewRequireUIDataClass)
	{
		RequiredUIDataClass = NewRequireUIDataClass;

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RequiredUIDataClass);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffects);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffectViewModels);
	}
}

TArray<FActiveGameplayEffectHandle> UVM_ActiveGameplayEffects::GetActiveEffects() const
{
	TArray<FActiveGameplayEffectHandle> Result;
	if (AbilitySystem.IsValid())
	{
		for (FActiveGameplayEffectsContainer::ConstIterator EffectIt = AbilitySystem->GetActiveGameplayEffects().CreateConstIterator(); EffectIt; ++EffectIt)
		{
			const FActiveGameplayEffect& Effect = *EffectIt;
			if (!EffectQuery.Matches(Effect))
			{
				continue;
			}
			if (RequiredUIDataClass && (!Effect.Spec.Def || !Effect.Spec.Def->FindComponent(RequiredUIDataClass)))
			{
				continue;
			}
			Result.Add(Effect.Handle);
		}
	}
	return Result;
}

TArray<UVM_ActiveGameplayEffect*> UVM_ActiveGameplayEffects::GetActiveEffectViewModels() const
{
	TArray<UVM_ActiveGameplayEffect*> Result;
	TArray<FActiveGameplayEffectHandle> ActiveEffects = GetActiveEffects();
	if (!ActiveEffects.IsEmpty())
	{
		UVM_ActiveGameplayEffects* MutableThis = const_cast<UVM_ActiveGameplayEffects*>(this);

		Result.Reserve(ActiveEffects.Num());
		for (const FActiveGameplayEffectHandle& ActiveEffect : ActiveEffects)
		{
			UVM_ActiveGameplayEffect* EffectViewModel = NewObject<UVM_ActiveGameplayEffect>(MutableThis, NAME_None, RF_Transient);
			EffectViewModel->SetActiveEffectHandle(ActiveEffect);
			Result.Add(EffectViewModel);
		}
	}
	return Result;
}

void UVM_ActiveGameplayEffects::PreSystemChange()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystem<UAbilitySystemComponent>())
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}

	Super::PreSystemChange();
}

void UVM_ActiveGameplayEffects::PostSystemChange()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystem<UAbilitySystemComponent>())
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UVM_ActiveGameplayEffects::OnActiveGameplayEffectAdded);
		ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UVM_ActiveGameplayEffects::OnAnyGameplayEffectRemoved);
	}

	Super::PostSystemChange();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffects);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffectViewModels);
}

void UVM_ActiveGameplayEffects::OnActiveGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent,
                                                            const FGameplayEffectSpec& GameplayEffectSpec,
                                                            FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	if (AbilitySystem.IsValid())
	{
		const FActiveGameplayEffect* ActiveEffect = AbilitySystem->GetActiveGameplayEffect(ActiveGameplayEffectHandle);
		if (ActiveEffect && EffectQuery.Matches(*ActiveEffect))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffects);
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffectViewModels);
		}
	}
}

void UVM_ActiveGameplayEffects::OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	if (EffectQuery.Matches(ActiveGameplayEffect))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffects);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveEffectViewModels);
	}
}
