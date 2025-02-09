// Copyright Bohdon Sayre, All Rights Reserved.


#include "HPAttributeSet.h"

#include "ExtendedAbilitySystemStatics.h"
#include "ExtendedCommonAbilitiesTags.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


UHPAttributeSet::UHPAttributeSet()
{
	InitHP(100);
	InitMaxHP(100);
}

void UHPAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHPAttribute())
	{
		// clamp to Max HP
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHP());
	}
	else if (Attribute == GetMaxHPAttribute())
	{
		// always keep Max HP rounded and >= 1
		NewValue = FMath::RoundFromZero(FMath::Max(NewValue, 1.f));
	}
}

void UHPAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// apply clamping to base values
	ClampAttribute(Attribute, NewValue);
}

void UHPAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// also apply clamping to current values (changed by temporary modifiers)
	ClampAttribute(Attribute, NewValue);
}

void UHPAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxHPAttribute())
	{
		// if MaxHP changes, increase/decrease HP to keep the same proportion (e.g. 80% of max)
		UExtendedAbilitySystemStatics::AdjustProportionalAttribute(GetOwningAbilitySystemComponent(), GetHPAttribute(), OldValue, NewValue, true);
	}
}

void UHPAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayTagContainer SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	if (Data.EvaluatedData.Magnitude < 0.f && SourceTags.HasTag(ExtendedCommonAbilitiesTags::TAG_Effect_Type_Damage))
	{
		// non-zero damage was dealt, send Event.Damage.Outgoing to the ability system that caused it
		if (UAbilitySystemComponent* SourceAbilitySystem = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent())
		{
			FGameplayEventData Payload;
			Payload.Target = SourceAbilitySystem->GetAvatarActor();
			Payload.EventTag = ExtendedCommonAbilitiesTags::TAG_Event_Damage_Outgoing;
			Payload.ContextHandle = Data.EffectSpec.GetContext();
			Payload.Instigator = Payload.ContextHandle.GetOriginalInstigator();
			Payload.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			// damage comes in as negative HP, so Magnitude will be negative,
			// use the positive value as the event magnitude.
			Payload.EventMagnitude = -Data.EvaluatedData.Magnitude;

			SourceAbilitySystem->HandleGameplayEvent(ExtendedCommonAbilitiesTags::TAG_Event_Damage_Outgoing, &Payload);
		}
	}
}

void UHPAttributeSet::OnRep_HP(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHPAttributeSet, HP, OldValue);
}

void UHPAttributeSet::OnRep_MaxHP(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHPAttributeSet, MaxHP, OldValue);
}

void UHPAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHPAttributeSet, HP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHPAttributeSet, MaxHP, COND_None, REPNOTIFY_Always);
}
