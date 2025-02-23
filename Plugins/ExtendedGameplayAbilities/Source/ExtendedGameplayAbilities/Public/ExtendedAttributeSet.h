// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ExtendedAttributeSet.generated.h"


USTRUCT(BlueprintType)
struct FExtendedMaxAttributeRules
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute MaxAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bProportional = false;
};


/**
 * AttributeSet with some built-in support for clamping and proportional value changes.
 */
UCLASS()
class EXTENDEDGAMEPLAYABILITIES_API UExtendedAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Map of attributes to their max value attributes, for attributed-based clamping. */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FExtendedMaxAttributeRules> MaxAttributesMap;

	/** Map of attributes to their min/max values, for non-attribute based clamping. */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FFloatRange> MinMaxValuesMap;

	/**
	 * Set an attribute to act as the maximum value for another attribute.
	 * @param Attribute The attribute to clamp.
	 * @param MaxAttribute The attribute representing the maximum value.
	 * @param bProportional When the max attribute changes, adjust the base attribute proportionally.
	 */
	void SetMaxAttribute(const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute, bool bProportional = false);

	/** Set the minimum and maximum values for an attribute. */
	void SetAttributeValueRange(const FGameplayAttribute& Attribute, float Min, float Max);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** Perform clamping specific to this attribute set. For use when either base or current value is changing. */
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	/**
	 * Perform clamping or proportional adjustments to a value, when a max attribute has changed.
	 * ex. increase HP when MaxHP goes up to maintain the same percent health.
	 */
	virtual void AdjustOrClampForMaxAttribute(const FGameplayAttribute& MaxAttribute, float OldMaxValue, float NewMaxValue);

	/** Set the default base and current value of an attribute. */
	static void InitAttribute(FGameplayAttributeData& AttributeData, float Value);
};
