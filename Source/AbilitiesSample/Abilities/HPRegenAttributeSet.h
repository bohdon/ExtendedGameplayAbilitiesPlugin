// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExtendedAttributeSet.h"
#include "ExtendedGameplayAttributeAccessors.h"
#include "HPRegenAttributeSet.generated.h"


/**
 * Attribute set for automatic health regeneration.
 */
UCLASS()
class ABILITIESSAMPLE_API UHPRegenAttributeSet : public UExtendedAttributeSet
{
	GENERATED_BODY()

public:
	UHPRegenAttributeSet();

	/** How much HP to regenerate each second. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HPRegen, Category = "Health")
	FGameplayAttributeData HPRegen;
	GAMEPLAYATTRIBUTE_ACCESSORS(UHPRegenAttributeSet, HPRegen)
	UFUNCTION()
	virtual void OnRep_HPRegen(FGameplayAttributeData& OldValue);
};
