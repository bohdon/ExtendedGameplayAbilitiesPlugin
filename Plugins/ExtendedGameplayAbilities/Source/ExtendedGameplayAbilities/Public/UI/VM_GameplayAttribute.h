// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemViewModelBase.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "VM_GameplayAttribute.generated.h"


/**
 * A viewmodel for displaying a gameplay attribute for an ability system.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UVM_GameplayAttribute : public UAbilitySystemViewModelBase
{
	GENERATED_BODY()

protected:
	/** The gameplay attribute. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayAttribute Attribute;

public:
	UFUNCTION(BlueprintSetter)
	void SetAttribute(FGameplayAttribute NewAttribute);

	UFUNCTION(BlueprintCallable)
	void SetAbilitySystemAndAttribute(UAbilitySystemComponent* NewAbilitySystem, FGameplayAttribute NewAttribute);

	FGameplayAttribute GetAttribute() const { return Attribute; }

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetValue() const;

protected:
	virtual void PreSystemChange() override;
	virtual void PostSystemChange() override;
	virtual void OnAttributeValueChanged(const FOnAttributeChangeData& ChangeData);
};
