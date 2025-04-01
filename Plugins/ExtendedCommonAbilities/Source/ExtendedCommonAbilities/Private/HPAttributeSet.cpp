// Copyright Bohdon Sayre, All Rights Reserved.


#include "HPAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "ExtendedCommonAbilitiesTags.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Net/UnrealNetwork.h"


UHPAttributeSet::UHPAttributeSet()
{
	InitAttribute(HP, 100.f);
	InitAttribute(MaxHP, 100.f);

	SetMaxAttribute(GetHPAttribute(), GetMaxHPAttribute(), true);
	SetAttributeValueRange(GetMaxHPAttribute(), 1.f, FLT_MAX);
}

void UHPAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayTagContainer SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	if (Data.EvaluatedData.Magnitude < 0.f && SourceTags.HasTag(ExtendedCommonAbilitiesTags::TAG_Effect_Type_Damage))
	{
		// non-zero damage was dealt, send Event.Damage.Incoming/Outgoing to the receiver/instigator
		BroadcastDamageEvent(GetOwningAbilitySystemComponent(), ExtendedCommonAbilitiesTags::TAG_Event_Damage_Incoming, Data);
		BroadcastDamageEvent(Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent(), ExtendedCommonAbilitiesTags::TAG_Event_Damage_Outgoing, Data);
	}
}

void UHPAttributeSet::BroadcastDamageEvent(UAbilitySystemComponent* TargetAbilitySystem,
                                           const FGameplayTag& EventTag,
                                           const FGameplayEffectModCallbackData& Data)
{
	if (!TargetAbilitySystem)
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.Target = TargetAbilitySystem->GetAvatarActor();
	Payload.EventTag = EventTag;

	Payload.ContextHandle = Data.EffectSpec.GetContext();
	Payload.Instigator = Payload.ContextHandle.GetOriginalInstigator();
	Payload.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
	// damage comes in as negative HP, so Magnitude will be negative,
	// use the positive value as the event magnitude.
	Payload.EventMagnitude = -Data.EvaluatedData.Magnitude;

	TargetAbilitySystem->HandleGameplayEvent(EventTag, &Payload);
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
