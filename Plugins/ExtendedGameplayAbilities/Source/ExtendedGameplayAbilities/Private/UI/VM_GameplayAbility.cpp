// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_GameplayAbility.h"

#include "AbilitySystemComponent.h"


void UVM_GameplayAbility::SetAbilitySpecHandle(FGameplayAbilitySpecHandle NewAbilitySpecHandle)
{
	SetAbilitySystemAndSpecHandle(AbilitySystem.Get(), NewAbilitySpecHandle);
}

void UVM_GameplayAbility::SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem)
{
	SetAbilitySystemAndSpecHandle(NewAbilitySystem, AbilitySpecHandle);
}

void UVM_GameplayAbility::SetAbilitySystemAndSpecHandle(UAbilitySystemComponent* NewAbilitySystem, FGameplayAbilitySpecHandle NewAbilitySpecHandle)
{
	if (AbilitySystem.Get() == NewAbilitySystem && AbilitySpecHandle == NewAbilitySpecHandle)
	{
		return;
	}

	if (AbilitySystem.IsValid())
	{
		AbilitySystem->AbilityActivatedCallbacks.RemoveAll(this);
		AbilitySystem->OnAbilityEnded.RemoveAll(this);
		AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		for (const FGameplayTag& CooldownTag : RegisteredCooldownTags)
		{
			AbilitySystem->RegisterGameplayTagEvent(CooldownTag).RemoveAll(this);
		}
	}

	AbilitySystem = NewAbilitySystem;
	AbilitySpecHandle = NewAbilitySpecHandle;

	if (AbilitySystem.IsValid())
	{
		// listen for ability activation
		AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &UVM_GameplayAbility::OnAnyAbilityActivated);
		AbilitySystem->OnAbilityEnded.AddUObject(this, &UVM_GameplayAbility::OnAnyAbilityEnded);
		// listen for cooldown effects being applied
		AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UVM_GameplayAbility::OnActiveGameplayEffectAdded);
		// listen for cooldown tags to update IsOnCooldown
		RegisteredCooldownTags = GetCooldownTags();
		for (const FGameplayTag& CooldownTag : RegisteredCooldownTags)
		{
			AbilitySystem->RegisterGameplayTagEvent(CooldownTag).AddUObject(this, &UVM_GameplayAbility::OnCooldownTagChanged);
		}
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilitySpecHandle);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilitySystem);

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityCDO);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityClass);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveCooldownEffect);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCooldownTags);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HasAbility);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsActive);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsOnCooldown);
}

bool UVM_GameplayAbility::HasAbility() const
{
	return GetAbilitySpec() != nullptr;
}

bool UVM_GameplayAbility::IsActive() const
{
	if (bIsActivating)
	{
		return true;
	}
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		return AbilitySpec->IsActive();
	}
	return false;
}

bool UVM_GameplayAbility::IsOnCooldown() const
{
	if (AbilitySystem.IsValid())
	{
		const FGameplayTagContainer CooldownTags = GetCooldownTags();
		if (!CooldownTags.IsEmpty())
		{
			return AbilitySystem->HasAnyMatchingGameplayTags(CooldownTags);
		}
	}
	return false;
}

FGameplayTagContainer UVM_GameplayAbility::GetCooldownTags() const
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (AbilitySpec && AbilitySpec->Ability)
	{
		if (const FGameplayTagContainer* CooldownTags = AbilitySpec->Ability->GetCooldownTags())
		{
			return *CooldownTags;
		}
	}
	return FGameplayTagContainer();
}

FActiveGameplayEffectHandle UVM_GameplayAbility::GetActiveCooldownEffect() const
{
	const FGameplayTagContainer CooldownTags = GetCooldownTags();
	if (!CooldownTags.IsEmpty())
	{
		const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);

		FActiveGameplayEffectHandle BestEffect;
		float BestEndTime = 0.f;

		for (FActiveGameplayEffectsContainer::ConstIterator EffectIt = AbilitySystem->GetActiveGameplayEffects().CreateConstIterator(); EffectIt; ++EffectIt)
		{
			const FActiveGameplayEffect& Effect = *EffectIt;
			if (Query.Matches(Effect))
			{
				const float EndTime = Effect.GetEndTime();
				if (!BestEffect.IsValid() || EndTime > BestEndTime)
				{
					BestEffect = Effect.Handle;
					BestEndTime = EndTime;
				}
			}
		}
		return BestEffect;
	}
	return FActiveGameplayEffectHandle();
}

const UGameplayAbility* UVM_GameplayAbility::GetAbilityCDO() const
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		return AbilitySpec->Ability;
	}
	return nullptr;
}

TSubclassOf<UGameplayAbility> UVM_GameplayAbility::GetAbilityClass() const
{
	if (const UGameplayAbility* AbilityCDO = GetAbilityCDO())
	{
		return AbilityCDO->GetClass();
	}
	return nullptr;
}

FGameplayAbilitySpec* UVM_GameplayAbility::GetAbilitySpec() const
{
	if (AbilitySystem.IsValid() && AbilitySpecHandle.IsValid())
	{
		return AbilitySystem->FindAbilitySpecFromHandle(AbilitySpecHandle);
	}
	return nullptr;
}

void UVM_GameplayAbility::OnAnyAbilityActivated(UGameplayAbility* GameplayAbility)
{
	if (GameplayAbility->GetClass() == GetAbilityClass())
	{
		TGuardValue<bool> IsActivatingGuard(bIsActivating, true);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsActive);
	}
}

void UVM_GameplayAbility::OnAnyAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle == AbilitySpecHandle)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsActive);
	}
}

void UVM_GameplayAbility::OnCooldownTagChanged(FGameplayTag GameplayTag, int32 NewCount)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsOnCooldown);
}

void UVM_GameplayAbility::OnActiveGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent,
                                                      const FGameplayEffectSpec& GameplayEffectSpec,
                                                      FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	if (AbilitySystem.Get() == AbilitySystemComponent)
	{
		FGameplayTagContainer GrantedTags;
		GameplayEffectSpec.GetAllGrantedTags(GrantedTags);
		if (GrantedTags.HasAny(GetCooldownTags()))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetActiveCooldownEffect);
			OnCooldownEffectAppliedEvent.Broadcast(ActiveGameplayEffectHandle);
		}
	}
}
