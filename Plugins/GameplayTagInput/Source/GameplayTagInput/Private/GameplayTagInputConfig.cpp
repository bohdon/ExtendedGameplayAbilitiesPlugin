// Copyright Bohdon Sayre, All Rights Reserved.


#include "GameplayTagInputConfig.h"

#include "GameplayTagInputModule.h"


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

	UE_LOG(LogGameplayTagInput, Error, TEXT("%s - No input action mapped to tag: %s"), *GetName(), *InputTag.ToString());

	return nullptr;
}
