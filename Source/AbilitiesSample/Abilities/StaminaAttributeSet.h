// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExtendedAttributeSet.h"
#include "ExtendedGameplayAttributeAccessors.h"
#include "StaminaAttributeSet.generated.h"


/**
 * Attribute set for Stamina
 */
UCLASS()
class ABILITIESSAMPLE_API UStaminaAttributeSet : public UExtendedAttributeSet
{
	GENERATED_BODY()

public:
	UStaminaAttributeSet();

	/** How much Stamina to regenerate each second. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Stamina")
	FGameplayAttributeData Stamina;
	GAMEPLAYATTRIBUTE_ACCESSORS(UStaminaAttributeSet, Stamina)
	UFUNCTION()
	virtual void OnRep_Stamina(FGameplayAttributeData& OldValue);

	/** How much Stamina to regenerate each second. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Stamina")
	FGameplayAttributeData MaxStamina;
	GAMEPLAYATTRIBUTE_ACCESSORS(UStaminaAttributeSet, MaxStamina)
	UFUNCTION()
	virtual void OnRep_MaxStamina(FGameplayAttributeData& OldValue);

	/** How much Stamina to regenerate each second. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegen, Category = "Stamina")
	FGameplayAttributeData StaminaRegen;
	GAMEPLAYATTRIBUTE_ACCESSORS(UStaminaAttributeSet, StaminaRegen)
	UFUNCTION()
	virtual void OnRep_StaminaRegen(FGameplayAttributeData& OldValue);
};
