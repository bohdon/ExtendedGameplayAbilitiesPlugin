// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_ActiveGameplayEffect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


void UVM_ActiveGameplayEffect::SetActiveEffectHandle(FActiveGameplayEffectHandle NewHandle)
{
	if (ActiveEffectHandle == NewHandle)
	{
		return;
	}

	ActiveEffectHandle = NewHandle;
	RemovalInfo = FGameplayEffectRemovalInfo();

	// TODO: cleanup old bindings
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		// ability system doesn't provide a way to get a mutable active effect,
		// but were only binding events, not changing anything important.
		// This is also the only way to access the OnInhibitionChanged event.
		FActiveGameplayEffect* MutableActiveEffect = const_cast<FActiveGameplayEffect*>(ActiveEffect);
		MutableActiveEffect->EventSet.OnEffectRemoved.AddUObject(this, &ThisClass::OnEffectRemoved);
		MutableActiveEffect->EventSet.OnStackChanged.AddUObject(this, &ThisClass::OnEffectStackChanged);
		MutableActiveEffect->EventSet.OnTimeChanged.AddUObject(this, &ThisClass::OnEffectTimeChanged);
		MutableActiveEffect->EventSet.OnInhibitionChanged.AddUObject(this, &ThisClass::OnEffectInhibitionChanged);
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ActiveEffectHandle);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStackCount);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStartTime);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDuration);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetEndTime);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsInhibited);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemovalInfo);
}

int32 UVM_ActiveGameplayEffect::GetStackCount() const
{
	return UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackCount(ActiveEffectHandle);
}

float UVM_ActiveGameplayEffect::GetStartTime() const
{
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		return ActiveEffect->StartWorldTime;
	}
	return 0.f;
}

float UVM_ActiveGameplayEffect::GetDuration() const
{
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		return ActiveEffect->GetDuration();
	}
	return 0.f;
}

float UVM_ActiveGameplayEffect::GetEndTime() const
{
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		return ActiveEffect->GetEndTime();
	}
	return 0.f;
}

bool UVM_ActiveGameplayEffect::IsInhibited() const
{
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		return ActiveEffect->bIsInhibited;
	}
	return false;
}

const FActiveGameplayEffect* UVM_ActiveGameplayEffect::GetActiveGameplayEffect() const
{
	if (const UAbilitySystemComponent* AbilitySystem = ActiveEffectHandle.GetOwningAbilitySystemComponent())
	{
		return AbilitySystem->GetActiveGameplayEffect(ActiveEffectHandle);
	}
	return nullptr;
}

void UVM_ActiveGameplayEffect::OnEffectRemoved(const FGameplayEffectRemovalInfo& InRemovalInfo)
{
	RemovalInfo = InRemovalInfo;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStackCount);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemovalInfo);
}

void UVM_ActiveGameplayEffect::OnEffectStackChanged(FActiveGameplayEffectHandle Handle, int32 NewStackCount, int32 OldStackCount)
{
	if (Handle != ActiveEffectHandle)
	{
		return;
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStackCount);
}

void UVM_ActiveGameplayEffect::OnEffectTimeChanged(FActiveGameplayEffectHandle Handle, float NewStartTime, float NewDuration)
{
	if (Handle != ActiveEffectHandle)
	{
		return;
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDuration);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStartTime);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetEndTime);
}

void UVM_ActiveGameplayEffect::OnEffectInhibitionChanged(FActiveGameplayEffectHandle Handle, bool bNewIsInhibited)
{
	if (Handle != ActiveEffectHandle)
	{
		return;
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsInhibited);
}
