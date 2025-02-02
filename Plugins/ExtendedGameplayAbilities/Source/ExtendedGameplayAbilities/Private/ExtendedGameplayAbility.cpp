// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedGameplayAbility.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "ExtendedAbilitySystemComponent.h"
#include "ExtendedAbilitySystemStatics.h"
#include "ExtendedGameplayAbilitiesSettings.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"


UExtendedGameplayAbility::UExtendedGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bActivateWhenGranted(false),
	  bHasDynamicCooldown(false)
{
}

float UExtendedGameplayAbility::GetDynamicCooldownDuration_Implementation(int32 AbilityLevel) const
{
	return DynamicCooldown.Duration.GetValueAtLevel(AbilityLevel);
}

UGameplayEffect* UExtendedGameplayAbility::GetCooldownGameplayEffect() const
{
	if (bHasDynamicCooldown)
	{
		if (DynamicCooldown.EffectClass)
		{
			return DynamicCooldown.EffectClass->GetDefaultObject<UGameplayEffect>();
		}
		// fall back to plugin settings
		const UExtendedGameplayAbilitiesSettings* Settings = GetDefault<UExtendedGameplayAbilitiesSettings>();
		if (Settings->DefaultDynamicCooldownEffectClass)
		{
			return Settings->DefaultDynamicCooldownEffectClass->GetDefaultObject<UGameplayEffect>();
		}
		return nullptr;
	}
	return Super::GetCooldownGameplayEffect();
}

const FGameplayTagContainer* UExtendedGameplayAbility::GetCooldownTags() const
{
	if (bHasDynamicCooldown)
	{
		return &DynamicCooldown.Tags;
	}
	return Super::GetCooldownTags();
}

void UExtendedGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (bHasDynamicCooldown)
	{
		const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
		if (!CooldownEffect)
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("%s: Dynamic cooldown is missing an EffectClass, and none is set in plugin settings."), *GetName());
			return;
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		// validate the cooldown effect
		UE_CLOG(CooldownEffect->DurationPolicy != EGameplayEffectDurationType::HasDuration, LogAbilitySystem, Warning,
		        TEXT("%s: Dynamic cooldown effect duration policy must be 'Has Duration: %s"),
		        *GetName(), *CooldownEffect->GetName());
		UE_CLOG(DynamicCooldown.Tags.IsEmpty(), LogAbilitySystem, Warning,
		        TEXT("%s: No dynamic cooldown tags were set"),
		        *GetName());
#endif

		const int32 AbilityLevel = GetAbilityLevel(Handle, ActorInfo);
		const float Duration = GetDynamicCooldownDuration(AbilityLevel);

		// skip cooldown if duration is nearly zero, since the effect would be treated as infinite
		if (Duration > SMALL_NUMBER)
		{
			const FGameplayEffectSpecHandle CooldownSpec = MakeOutgoingGameplayEffectSpec(CooldownEffect->GetClass(), AbilityLevel);
			CooldownSpec.Data->DynamicGrantedTags.AppendTags(DynamicCooldown.Tags);
			CooldownSpec.Data->SetDuration(Duration, /*bLockDuration*/ true);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpec);
		}
	}
	else
	{
		Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	}
}

void UExtendedGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateWhenGranted && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}

	OnAvatarSet_BP();
}

FGameplayEffectSet UExtendedGameplayAbility::GetEffectSet(FGameplayTag Tag) const
{
	return EffectSetMap.FindRef(Tag);
}

FGameplayEffectSpecSet UExtendedGameplayAbility::MakeEffectSpecSet(const FGameplayEffectSet& EffectSet, int32 OverrideGameplayLevel)
{
	FGameplayEffectSpecSet Result;

	if (OverrideGameplayLevel == INDEX_NONE)
	{
		OverrideGameplayLevel = GetAbilityLevel();
	}

	const UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningActorFromActorInfo());
	if (!AbilitySystem)
	{
		return Result;
	}

	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectSet.Effects)
	{
		FGameplayEffectSpecHandle NewEffectSpec = MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel);
		if (NewEffectSpec.IsValid())
		{
			for (const auto& Item : EffectSet.SetByCallerMagnitudes)
			{
				const float Value = Item.Value.GetValueAtLevel(OverrideGameplayLevel);
				NewEffectSpec.Data->SetByCallerTagMagnitudes.Add(Item.Key, Value);
			}

			Result.EffectSpecs.Add(NewEffectSpec);
		}
	}

	return Result;
}

FGameplayEffectSpecSet UExtendedGameplayAbility::MakeEffectSpecSetByTag(FGameplayTag Tag, int32 OverrideGameplayLevel)
{
	if (EffectSetMap.Contains(Tag))
	{
		const FGameplayEffectSet& EffectSet = EffectSetMap[Tag];
		if (!EffectSet.IsEmpty())
		{
			return MakeEffectSpecSet(EffectSet, OverrideGameplayLevel);
		}
	}

	return FGameplayEffectSpecSet();
}

TArray<FActiveGameplayEffectHandle> UExtendedGameplayAbility::ApplyEffectSpecSetToOwner_BP(const FGameplayEffectSpecSet& EffectSpecSet)
{
	return ApplyEffectSpecSetToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecSet);
}

TArray<FActiveGameplayEffectHandle> UExtendedGameplayAbility::ApplyEffectSpecSetToOwner(const FGameplayAbilitySpecHandle AbilityHandle,
                                                                                        const FGameplayAbilityActorInfo* ActorInfo,
                                                                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                                                                        const FGameplayEffectSpecSet& EffectSpecSet)
{
	if (EffectSpecSet.IsEmpty() || !HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return TArray<FActiveGameplayEffectHandle>();
	}

	UExtendedAbilitySystemComponent* ExtendedAbilitySystem = Cast<UExtendedAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!ExtendedAbilitySystem)
	{
		// extended ability system component is required
		UE_LOG(LogAbilitySystem, Error, TEXT("%s cannot apply effect spec set on %s, ability system must inherit UExtendedAbilitySystemComponent."),
		       *GetName(), *GetNameSafe(ActorInfo->AvatarActor.Get()));
		return TArray<FActiveGameplayEffectHandle>();
	}

	return ExtendedAbilitySystem->ApplyGameplayEffectSpecSetToSelf(EffectSpecSet);
}

float UExtendedGameplayAbility::GetAbilityStat(FDataRegistryId Id, float DefaultValue) const
{
	const float InputValue = static_cast<float>(GetAbilityLevel());
	return UExtendedAbilitySystemStatics::GetDataRegistryValue(Id, InputValue, DefaultValue);
}

APawn* UExtendedGameplayAbility::GetPawnFromActorInfo() const
{
	return Cast<APawn>(GetAvatarActorFromActorInfo());
}

ACharacter* UExtendedGameplayAbility::GetCharacterFromActorInfo() const
{
	return Cast<ACharacter>(GetAvatarActorFromActorInfo());
}

AController* UExtendedGameplayAbility::GetControllerFromActorInfo() const
{
	if (!CurrentActorInfo)
	{
		return nullptr;
	}

	if (CurrentActorInfo->PlayerController.IsValid())
	{
		return CurrentActorInfo->PlayerController.Get();
	}

	// search up the chain of owners for any controller
	AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
	while (TestActor)
	{
		if (AController* Controller = Cast<AController>(TestActor))
		{
			return Controller;
		}

		if (const APawn* Pawn = Cast<APawn>(TestActor))
		{
			return Pawn->GetController();
		}

		TestActor = TestActor->GetOwner();
	}

	return nullptr;
}

APlayerController* UExtendedGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	return Cast<APlayerController>(GetControllerFromActorInfo());
}
