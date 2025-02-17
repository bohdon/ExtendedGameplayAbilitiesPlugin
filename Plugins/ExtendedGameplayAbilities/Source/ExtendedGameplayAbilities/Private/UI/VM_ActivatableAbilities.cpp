// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_ActivatableAbilities.h"

#include "AbilitySystemComponent.h"
#include "ExtendedAbilitySystemComponent.h"
#include "UI/VM_GameplayAbility.h"


void UVM_ActivatableAbilities::SetAbilitySystem(UExtendedAbilitySystemComponent* NewAbilitySystem)
{
	if (AbilitySystem.Get() == NewAbilitySystem)
	{
		return;
	}

	if (AbilitySystem.IsValid())
	{
		AbilitySystem->OnGiveAbilityEvent.RemoveAll(this);
		AbilitySystem->OnRemoveAbilityEvent.RemoveAll(this);
	}

	AbilitySystem = NewAbilitySystem;

	if (AbilitySystem.IsValid())
	{
		AbilitySystem->OnGiveAbilityEvent.AddUObject(this, &UVM_ActivatableAbilities::OnGiveAbility);
		AbilitySystem->OnRemoveAbilityEvent.AddUObject(this, &UVM_ActivatableAbilities::OnRemoveAbility);
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilitySystem);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilitySpecHandles);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityViewModels);
	OnAbilitiesChangedEvent.Broadcast();
}

void UVM_ActivatableAbilities::SetAbilityTagQuery(const FGameplayTagQuery& NewTagQuery)
{
	if (AbilityTagQuery == NewTagQuery)
	{
		return;
	}

	AbilityTagQuery = NewTagQuery;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AbilityTagQuery);

	if (AbilitySystem.IsValid())
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilitySpecHandles);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityViewModels);
		OnAbilitiesChangedEvent.Broadcast();
	}
}

TArray<FGameplayAbilitySpecHandle> UVM_ActivatableAbilities::GetAbilitySpecHandles() const
{
	TArray<FGameplayAbilitySpecHandle> Result;
	if (AbilitySystem.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystem->GetActivatableAbilities())
		{
			if (ShouldIncludeAbility(AbilitySpec))
			{
				Result.Add(AbilitySpec.Handle);
			}
		}
	}
	return Result;
}


TArray<UVM_GameplayAbility*> UVM_ActivatableAbilities::GetAbilityViewModels() const
{
	TArray<UVM_GameplayAbility*> Result;
	if (AbilitySystem.IsValid())
	{
		UExtendedAbilitySystemComponent* ASC = AbilitySystem.Get();
		UVM_ActivatableAbilities* MutableThis = const_cast<UVM_ActivatableAbilities*>(this);

		Result.Reserve(ASC->GetActivatableAbilities().Num());
		for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
		{
			if (ShouldIncludeAbility(AbilitySpec))
			{
				UVM_GameplayAbility* AbilityViewModel = NewObject<UVM_GameplayAbility>(MutableThis);
				AbilityViewModel->SetAbilitySystemAndSpecHandle(ASC, AbilitySpec.Handle);
				Result.Add(AbilityViewModel);
			}
		}
	}
	return Result;
}

FGameplayAbilitySpecHandle UVM_ActivatableAbilities::FindAbilityMatchingTags(FGameplayTagContainer MatchingTags, FGameplayTagContainer IgnoreTags) const
{
	if (AbilitySystem.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystem->GetActivatableAbilities())
		{
			if (ShouldIncludeAbility(AbilitySpec))
			{
				FGameplayTagContainer AbilityTags;
				AbilityTags.AppendTags(AbilitySpec.Ability->GetAssetTags());
				AbilityTags.AppendTags(AbilitySpec.GetDynamicSpecSourceTags());
				if (AbilityTags.HasAny(MatchingTags) && !AbilityTags.HasAny(IgnoreTags))
				{
					return AbilitySpec.Handle;
				}
			}
		}
	}
	return FGameplayAbilitySpecHandle();
}

bool UVM_ActivatableAbilities::ShouldIncludeAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	if (!AbilitySpec.Ability || AbilitySpec.Handle == AbilityBeingRemoved)
	{
		return false;
	}

	if (!AbilityTagQuery.IsEmpty())
	{
		FGameplayTagContainer AbilityTags;
		AbilityTags.AppendTags(AbilitySpec.Ability->GetAssetTags());
		AbilityTags.AppendTags(AbilitySpec.GetDynamicSpecSourceTags());
		if (!AbilityTagQuery.Matches(AbilityTags))
		{
			return false;
		}
	}

	return true;
}

void UVM_ActivatableAbilities::OnGiveAbility(FGameplayAbilitySpec& GameplayAbilitySpec)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilitySpecHandles);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityViewModels);
	OnAbilitiesChangedEvent.Broadcast();
}

void UVM_ActivatableAbilities::OnRemoveAbility(FGameplayAbilitySpec& GameplayAbilitySpec)
{
	AbilityBeingRemoved = GameplayAbilitySpec.Handle;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilitySpecHandles);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityViewModels);
	OnAbilitiesChangedEvent.Broadcast();

	AbilityBeingRemoved = FGameplayAbilitySpecHandle();
}
