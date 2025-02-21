// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/VM_ActiveGameplayEffect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectUIData.h"
#include "Engine/World.h"


UVM_ActiveGameplayEffect::UVM_ActiveGameplayEffect()
{
	UIDataClass = UGameplayEffectUIData::StaticClass();
}

void UVM_ActiveGameplayEffect::SetActiveEffectHandle(FActiveGameplayEffectHandle NewHandle)
{
	if (ActiveEffectHandle == NewHandle)
	{
		return;
	}

	if (FActiveGameplayEffectEvents* EventSet = GetActiveEffectEventSet())
	{
		EventSet->OnEffectRemoved.RemoveAll(this);
		EventSet->OnStackChanged.RemoveAll(this);
		EventSet->OnTimeChanged.RemoveAll(this);
		EventSet->OnInhibitionChanged.RemoveAll(this);
	}

	ActiveEffectHandle = NewHandle;
	RemovalInfo = FGameplayEffectRemovalInfo();

	if (FActiveGameplayEffectEvents* EventSet = GetActiveEffectEventSet())
	{
		EventSet->OnEffectRemoved.AddUObject(this, &ThisClass::OnEffectRemoved);
		EventSet->OnStackChanged.AddUObject(this, &ThisClass::OnEffectStackChanged);
		EventSet->OnTimeChanged.AddUObject(this, &ThisClass::OnEffectTimeChanged);
		EventSet->OnInhibitionChanged.AddUObject(this, &ThisClass::OnEffectInhibitionChanged);
	}

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ActiveEffectHandle);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDuration);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetEndTime);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStackCount);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStartTime);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetUIData);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsInhibited);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemovalInfo);
}

void UVM_ActiveGameplayEffect::SetUIDataClass(TSubclassOf<UGameplayEffectUIData> NewUIDataClass)
{
	if (NewUIDataClass && UIDataClass != NewUIDataClass)
	{
		UIDataClass = NewUIDataClass;

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(UIDataClass);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetUIData);
	}
}

const UGameplayEffectUIData* UVM_ActiveGameplayEffect::GetUIData() const
{
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		return Cast<UGameplayEffectUIData>(ActiveEffect->Spec.Def->FindComponent(UIDataClass));
	}
	return nullptr;
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

float UVM_ActiveGameplayEffect::GetTimeRemaining() const
{
	return FMath::Max(GetEndTime() - GetWorld()->GetTimeSeconds(), 0.f);
}

float UVM_ActiveGameplayEffect::GetNormalizedTimeRemaining() const
{
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect())
	{
		const float Duration = ActiveEffect->GetDuration();
		const float TimeRemaining = FMath::Max(ActiveEffect->GetEndTime() - GetWorld()->GetTimeSeconds(), 0.f);
		return Duration > UE_SMALL_NUMBER ? FMath::Clamp(TimeRemaining / Duration, 0.f, 1.f) : 0.f;
	}
	return 0.f;
}

const FActiveGameplayEffect* UVM_ActiveGameplayEffect::GetActiveGameplayEffect() const
{
	if (const UAbilitySystemComponent* AbilitySystem = ActiveEffectHandle.GetOwningAbilitySystemComponent())
	{
		return AbilitySystem->GetActiveGameplayEffect(ActiveEffectHandle);
	}
	return nullptr;
}

FActiveGameplayEffectEvents* UVM_ActiveGameplayEffect::GetActiveEffectEventSet() const
{
	if (UAbilitySystemComponent* AbilitySystem = ActiveEffectHandle.GetOwningAbilitySystemComponent())
	{
		return AbilitySystem->GetActiveEffectEventSet(ActiveEffectHandle);
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
