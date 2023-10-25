// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedAbilitySystemStatics.h"

#include "AbilitySystemGlobals.h"
#include "DataRegistrySubsystem.h"
#include "ExtendedAbilitySystemComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Phases/AbilityGamePhaseComponent.h"


UExtendedAbilitySystemComponent* UExtendedAbilitySystemStatics::GetExtendedAbilitySystemComponent(AActor* Actor)
{
	return Cast<UExtendedAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor));
}

FGameplayEffectSpecSet UExtendedAbilitySystemStatics::AssignTagSetByCallerMagnitudeForSet(FGameplayEffectSpecSet EffectSpecSet, FGameplayTag DataTag,
                                                                                          float Magnitude)
{
	for (FGameplayEffectSpecHandle SpecHandle : EffectSpecSet.EffectSpecs)
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Spec)
		{
			Spec->SetSetByCallerMagnitude(DataTag, Magnitude);
		}
	}
	return EffectSpecSet;
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

TArray<FActiveGameplayEffectHandle> UExtendedAbilitySystemStatics::GetActiveEffectsGrantingGameplayCue(UAbilitySystemComponent* AbilitySystem,
                                                                                                       FGameplayTag GameplayCueTag)
{
	if (!AbilitySystem)
	{
		return TArray<FActiveGameplayEffectHandle>();
	}

	FGameplayEffectQuery Query;
	Query.CustomMatchDelegate = FActiveGameplayEffectQueryCustomMatch::CreateLambda([&GameplayCueTag](const FActiveGameplayEffect& ActiveEffect)
	{
		return ActiveEffect.Spec.Def->GameplayCues.ContainsByPredicate([&GameplayCueTag](const FGameplayEffectCue& EffectCue)
		{
			return EffectCue.GameplayCueTags.HasTag(GameplayCueTag);
		});
	});

	return AbilitySystem->GetActiveEffects(Query);
}

void UExtendedAbilitySystemStatics::AdjustProportionalAttribute(UAbilitySystemComponent* AbilitySystem, const FGameplayAttribute& Attribute,
                                                                float OldRelatedValue, float NewRelatedValue, bool bRound, bool bClamp)
{
	if (!AbilitySystem || FMath::IsNearlyEqual(OldRelatedValue, NewRelatedValue))
	{
		return;
	}

	const float CurrentValue = AbilitySystem->GetNumericAttributeBase(Attribute);

	float NewValue = OldRelatedValue > 0.f ? CurrentValue * (NewRelatedValue / OldRelatedValue) : NewRelatedValue;

	if (bRound)
	{
		NewValue = FMath::RoundFromZero(NewValue);
	}

	if (bClamp)
	{
		NewValue = FMath::Min(NewValue, NewRelatedValue);
	}

	AbilitySystem->SetNumericAttributeBase(Attribute, NewValue);
}

float UExtendedAbilitySystemStatics::GetDataRegistryValue(FDataRegistryId Id, float InputValue, float DefaultValue)
{
	const UDataRegistrySubsystem* DataRegistrySubsystem = UDataRegistrySubsystem::Get();

	float OutValue;
	const FRealCurve* FoundCurve = nullptr;
	if (DataRegistrySubsystem->EvaluateCachedCurve(OutValue, FoundCurve, Id, InputValue, DefaultValue))
	{
		return OutValue;
	}

	return DefaultValue;
}

UAbilityGamePhaseComponent* UExtendedAbilitySystemStatics::GetAbilityGamePhaseComponent(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->FindComponentByClass<UAbilityGamePhaseComponent>();
		}
	}
	return nullptr;
}
