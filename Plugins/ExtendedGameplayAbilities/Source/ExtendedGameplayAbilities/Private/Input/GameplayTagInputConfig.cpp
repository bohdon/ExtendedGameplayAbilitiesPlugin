// Copyright Bohdon Sayre, All Rights Reserved.


#include "Input/GameplayTagInputConfig.h"

#include "AbilitySystemLog.h"


UGameplayTagInputConfig::UGameplayTagInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UGameplayTagInputConfig::FindInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FGameplayTagInputAction& Action : InputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}
	return nullptr;
}
