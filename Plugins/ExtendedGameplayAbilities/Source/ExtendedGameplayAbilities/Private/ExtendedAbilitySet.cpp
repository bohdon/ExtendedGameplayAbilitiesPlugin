// Copyright Bohdon Sayre, All Rights Reserved.

#include "ExtendedAbilitySet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AttributeSet.h"


// FExtendedAbilitySetHandles
// --------------------------

void FExtendedAbilitySetHandles::Reset()
{
	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	AttributeSetClasses.Reset();
}

void FExtendedAbilitySetHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FExtendedAbilitySetHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FExtendedAbilitySetHandles::AddAttributeSet(const UAttributeSet* AttributeSet)
{
	if (IsValid(AttributeSet))
	{
		AttributeSetClasses.Add(AttributeSet->GetClass());
	}
}

FExtendedAbilitySetHandles UExtendedAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystem, UObject* SourceObject, int32 OverrideLevel) const
{
	check(AbilitySystem);

	if (!AbilitySystem->IsOwnerActorAuthoritative())
	{
		return FExtendedAbilitySetHandles();
	}

	FExtendedAbilitySetHandles Result;

	Result.AbilitySet = this;

	// give attribute sets first, since passive abilities or effects may need to capture attribute values
	for (int32 SetIdx = 0; SetIdx < GrantedAttributes.Num(); ++SetIdx)
	{
		const FExtendedAbilitySetAttributes& SetToGrant = GrantedAttributes[SetIdx];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set %s is not valid."),
			       SetIdx, *GetNameSafe(this));
			continue;
		}

		if (AbilitySystem->GetAttributeSet(SetToGrant.AttributeSet))
		{
			// attribute set already exists
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(AbilitySystem->GetOwner(), SetToGrant.AttributeSet);
		AbilitySystem->AddSpawnedAttribute(NewSet);

		Result.AddAttributeSet(NewSet);
	}

	// give abilities
	for (int32 AbilityIdx = 0; AbilityIdx < GrantedAbilities.Num(); ++AbilityIdx)
	{
		const FExtendedAbilitySetAbility& AbilityToGrant = GrantedAbilities[AbilityIdx];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("GrantedAbilities[%d] on ability set %s is not valid."),
			       AbilityIdx, *GetNameSafe(this));
			continue;
		}

		FGameplayAbilitySpec AbilitySpec = CreateAbilitySpec(AbilityToGrant, AbilitySystem, SourceObject, OverrideLevel);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystem->GiveAbility(AbilitySpec);
		Result.AddAbilitySpecHandle(AbilitySpecHandle);
	}

	// give effects
	for (int32 EffectIdx = 0; EffectIdx < GrantedEffects.Num(); ++EffectIdx)
	{
		const FExtendedAbilitySetEffect& EffectToGrant = GrantedEffects[EffectIdx];

		if (!IsValid(EffectToGrant.Effect))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("GrantedEffects[%d] on ability set %s is not valid."),
			       EffectIdx, *GetNameSafe(this));
			continue;
		}

		FGameplayEffectSpec EffectSpec = CreateEffectSpec(EffectToGrant, AbilitySystem, SourceObject, OverrideLevel);

		const FActiveGameplayEffectHandle EffectHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(EffectSpec);
		Result.AddGameplayEffectHandle(EffectHandle);
	}

	return Result;
}

void UExtendedAbilitySet::RemoveFromAbilitySystem(UAbilitySystemComponent* AbilitySystem,
                                                  FExtendedAbilitySetHandles& AbilitySetHandles,
                                                  bool bEndAbilities,
                                                  bool bKeepAttributeSets) const
{
	check(AbilitySystem);

	if (!AbilitySystem->IsOwnerActorAuthoritative())
	{
		return;
	}

	// remove abilities
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySetHandles.AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			if (bEndAbilities)
			{
				AbilitySystem->ClearAbility(Handle);
			}
			else
			{
				AbilitySystem->SetRemoveAbilityOnEnd(Handle);
			}
		}
	}

	// remove effects
	for (const FActiveGameplayEffectHandle& Handle : AbilitySetHandles.GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystem->RemoveActiveGameplayEffect(Handle);
		}
	}

	// remove granted attribute sets
	if (!bKeepAttributeSets)
	{
		for (const TSubclassOf<UAttributeSet>& AttributeSetClass : AbilitySetHandles.AttributeSetClasses)
		{
			if (UAttributeSet* AttributeSet = const_cast<UAttributeSet*>(AbilitySystem->GetAttributeSet(AttributeSetClass)))
			{
				AbilitySystem->RemoveSpawnedAttribute(AttributeSet);
			}
		}
	}

	AbilitySetHandles.Reset();
}

FGameplayAbilitySpec UExtendedAbilitySet::CreateAbilitySpec(const FExtendedAbilitySetAbility& AbilityToGrant, UAbilitySystemComponent* AbilitySystem,
                                                            UObject* SourceObject, int32 OverrideLevel) const
{
	check(AbilityToGrant.Ability);

	UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

	FGameplayAbilitySpec AbilitySpec(AbilityCDO, OverrideLevel >= 0 ? OverrideLevel : AbilityToGrant.Level);
	AbilitySpec.SourceObject = SourceObject;
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

	return AbilitySpec;
}

FGameplayEffectSpec UExtendedAbilitySet::CreateEffectSpec(const FExtendedAbilitySetEffect& EffectToGrant, UAbilitySystemComponent* AbilitySystem,
                                                          UObject* SourceObject, int32 OverrideLevel) const
{
	check(EffectToGrant.Effect);
	check(AbilitySystem);

	const UGameplayEffect* EffectCDO = EffectToGrant.Effect->GetDefaultObject<UGameplayEffect>();

	FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject);

	FGameplayEffectSpec EffectSpec(EffectCDO, ContextHandle, OverrideLevel >= 0 ? OverrideLevel : EffectToGrant.Level);

	return EffectSpec;
}
