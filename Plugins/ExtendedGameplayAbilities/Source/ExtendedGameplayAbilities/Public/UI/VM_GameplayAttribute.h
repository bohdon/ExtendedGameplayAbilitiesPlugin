// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "MVVMViewModelBase.h"
#include "VM_GameplayAttribute.generated.h"


/**
 * A viewmodel for displaying a gameplay attribute for an ability system.
 */
UCLASS(BlueprintType)
class EXTENDEDGAMEPLAYABILITIES_API UVM_GameplayAttribute : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	/** The gameplay attribute. */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FGameplayAttribute Attribute;

	/** The owning ability system. */
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

public:
	UFUNCTION(BlueprintSetter)
	void SetAttribute(FGameplayAttribute NewAttribute);

	UFUNCTION(BlueprintCallable)
	void SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem);

	UFUNCTION(BlueprintCallable)
	void SetAttributeAndAbilitySystem(FGameplayAttribute NewAttribute, UAbilitySystemComponent* NewAbilitySystem);

	UAbilitySystemComponent* GetAbilitySystem() const { return AbilitySystem.Get(); }

	FGameplayAttribute GetAttribute() const { return Attribute; }

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetValue() const;

protected:
	virtual void OnAttributeValueChanged(const FOnAttributeChangeData& ChangeData);
};
