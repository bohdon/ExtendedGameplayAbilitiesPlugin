// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedAbilitySystemStatics.h"

#include "AbilitySystemGlobals.h"
#include "ExtendedAbilitySystemComponent.h"


UExtendedAbilitySystemComponent* UExtendedAbilitySystemStatics::GetExtendedAbilitySystemComponent(AActor* Actor)
{
	return Cast<UExtendedAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor));
}

TArray<FActiveGameplayEffectHandle> UExtendedAbilitySystemStatics::ApplyEffectSpecSetToActor(AActor* Actor, const FGameplayEffectSpecSet& EffectSpecSet,
                                                                                             const UObject* SourceObject, bool& bSuccess)
{
	UExtendedAbilitySystemComponent* ExtendedAbilitySystem = GetExtendedAbilitySystemComponent(Actor);
	if (!ExtendedAbilitySystem)
	{
		bSuccess = false;
		return TArray<FActiveGameplayEffectHandle>();
	}

	if (SourceObject)
	{
		// update the source object for all effect specs, this is a lasting change for those specs
		for (const FGameplayEffectSpecHandle& SpecHandle : EffectSpecSet.EffectSpecs)
		{
			if (SpecHandle.IsValid())
			{
				// 'Add' is misleading here, there is only one source object and this replaces it
				SpecHandle.Data->GetContext().AddSourceObject(SourceObject);
			}
		}
	}

	bSuccess = true;
	return ExtendedAbilitySystem->ApplyGameplayEffectSpecSetToSelf(EffectSpecSet);
}

TArray<FActiveGameplayEffectHandle> UExtendedAbilitySystemStatics::ApplyEffectSpecSetToActorOnce(AActor* Actor, const FGameplayEffectSpecSet& EffectSpecSet,
                                                                                                 const UObject* SourceObject, TArray<AActor*>& AffectedActors,
                                                                                                 bool& bSuccess)
{
	if (AffectedActors.Contains(Actor))
	{
		bSuccess = false;
		return TArray<FActiveGameplayEffectHandle>();
	}

	TArray<FActiveGameplayEffectHandle> Result = ApplyEffectSpecSetToActor(Actor, EffectSpecSet, SourceObject, bSuccess);
	if (bSuccess)
	{
		AffectedActors.Add(Actor);
	}
	return Result;
}

int32 UExtendedAbilitySystemStatics::RemoveEffectsFromActorBySourceObject(AActor* Actor, const UObject* SourceObject, TArray<AActor*>& AffectedActors)
{
	if (AffectedActors.Contains(Actor))
	{
		AffectedActors.Remove(Actor);
	}

	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		FGameplayEffectQuery EffectQuery;
		EffectQuery.EffectSource = SourceObject;

		return AbilitySystem->RemoveActiveEffects(EffectQuery);
	}

	return 0;
}
