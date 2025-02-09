// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ExtendedGameplayAttributeAccessors.h"
#include "HPAttributeSet.generated.h"


/**
 * An attribute set HP and MaxHP attributes.
 */
UCLASS()
class EXTENDEDCOMMONABILITIES_API UHPAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UHPAttributeSet();

	/** The current HP of a character. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HP, Category = "Health")
	FGameplayAttributeData HP;
	GAMEPLAYATTRIBUTE_ACCESSORS(UHPAttributeSet, HP)
	UFUNCTION()
	virtual void OnRep_HP(FGameplayAttributeData& OldValue);

	/** The maximum HP of a character. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHP, Category = "Health")
	FGameplayAttributeData MaxHP;
	GAMEPLAYATTRIBUTE_ACCESSORS(UHPAttributeSet, MaxHP)
	UFUNCTION()
	virtual void OnRep_MaxHP(FGameplayAttributeData& OldValue);

	/** Perform clamping specific to this attribute set. For use when either base or current value is changing. */
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
