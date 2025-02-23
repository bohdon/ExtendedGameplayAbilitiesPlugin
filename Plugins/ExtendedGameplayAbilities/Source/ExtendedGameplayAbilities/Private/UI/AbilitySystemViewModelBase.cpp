// Copyright Bohdon Sayre, All Rights Reserved.


#include "UI/AbilitySystemViewModelBase.h"

#include "AbilitySystemComponent.h"
#include "MVVMViewModelBase.h"
#include "Logging/MessageLog.h"


#define LOCTEXT_NAMESPACE "ExtendedGameplayAbilities"

void UAbilitySystemViewModelBase::SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem)
{
#if !NO_LOGGING
	if (RequireAbilitySystemClass)
	{
		// only allow UExtendedAbilitySystemComponent
		if (!NewAbilitySystem->IsA(RequireAbilitySystemClass))
		{
			FMessageLog("PIE").Warning(
				FText::Format(LOCTEXT("AbilitySystemVMWrongClass",
				                      "{0}: SetAbilitySystem called with the wrong type, expected a {1}"),
				              FText::FromString(GetClass()->GetName()),
				              FText::FromString(RequireAbilitySystemClass->GetName())));
		}
	}
#endif

	if (AbilitySystem.Get() != NewAbilitySystem)
	{
		PreSystemChange();
		AbilitySystem = NewAbilitySystem;
		PostSystemChange();
	}
}

void UAbilitySystemViewModelBase::PreSystemChange()
{
}

void UAbilitySystemViewModelBase::PostSystemChange()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilitySystem);
}

#undef LOCTEXT_NAMESPACE
