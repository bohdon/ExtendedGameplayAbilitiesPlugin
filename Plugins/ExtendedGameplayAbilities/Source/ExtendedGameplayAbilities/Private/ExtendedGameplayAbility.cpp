// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedGameplayAbility.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "EnhancedInputSubsystems.h"
#include "ExtendedAbilitySystemComponent.h"
#include "ExtendedAbilitySystemStatics.h"
#include "ExtendedGameplayAbilitiesSettings.h"
#include "Components/InputComponent.h"
#include "Engine/InputDelegateBinding.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"


UExtendedGameplayAbility::UExtendedGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void UExtendedGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (bEnableInput)
	{
		// setup enhanced input support if needed
		InitializeInputComponent();
		UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent, this);
	}
}

void UExtendedGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bEnableInput)
	{
		UninitializeInputComponent();

		TArray<TObjectPtr<const UInputMappingContext>> MappingContextsToRemove = ActiveMappingContexts;
		for (const TObjectPtr<const UInputMappingContext>& MappingContext : MappingContextsToRemove)
		{
			RemoveInputMappingContext(MappingContext);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UExtendedGameplayAbility::InitializeInputComponent()
{
	if (GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		return;
	}

	if (APlayerController* Controller = GetPlayerControllerFromActorInfo())
	{
		const UClass* InputClass = Controller->InputComponent ? Controller->InputComponent->GetClass() : UInputSettings::GetDefaultInputComponentClass();
		InputComponent = NewObject<UInputComponent>(this, InputClass, NAME_None, RF_Transient);
		InputComponent->Priority = InputPriority;
		InputComponent->bBlockInput = bBlockInput;

		Controller->PushInputComponent(InputComponent);
	}
}

void UExtendedGameplayAbility::UninitializeInputComponent()
{
	if (GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		return;
	}

	if (InputComponent)
	{
		if (APlayerController* Controller = GetPlayerControllerFromActorInfo())
		{
			Controller->PopInputComponent(InputComponent);
		}

		InputComponent->ClearActionBindings();
		InputComponent->MarkAsGarbage();
		InputComponent = nullptr;
	}
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

bool UExtendedGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                                 const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                                                 FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UExtendedAbilitySystemComponent* ExtendedAbilitySystem = Cast<UExtendedAbilitySystemComponent>(&AbilitySystemComponent);
	if (!ExtendedAbilitySystem)
	{
		return true;
	}

	// check additional tag requirements from tag relationship mappings.
	FGameplayTagContainer AdditionalRequiredTags;
	FGameplayTagContainer AdditionalBlockedTags;
	ExtendedAbilitySystem->GetAdditionalActivationTagRequirements(GetAssetTags(), AdditionalRequiredTags, AdditionalBlockedTags);

	// lambdas below are copied from the parent function.

	// Define a common lambda to check for blocked tags
	bool bBlocked = false;
	auto CheckForBlocked = [&](const FGameplayTagContainer& ContainerA, const FGameplayTagContainer& ContainerB)
	{
		// Do we not have any tags in common?  Then we're not blocked
		if (ContainerA.IsEmpty() || ContainerB.IsEmpty() || !ContainerA.HasAny(ContainerB))
		{
			return;
		}

		if (OptionalRelevantTags)
		{
			// Ensure the global blocking tag is only added once
			if (!bBlocked)
			{
				UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
				const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
				OptionalRelevantTags->AddTag(BlockedTag);
			}

			// Now append all the blocking tags
			OptionalRelevantTags->AppendMatchingTags(ContainerA, ContainerB);
		}

		bBlocked = true;
	};

	// Define a common lambda to check for missing required tags
	bool bMissing = false;
	auto CheckForRequired = [&](const FGameplayTagContainer& TagsToCheck, const FGameplayTagContainer& RequiredTags)
	{
		// Do we have no requirements, or have met all requirements?  Then nothing's missing
		if (RequiredTags.IsEmpty() || TagsToCheck.HasAll(RequiredTags))
		{
			return;
		}

		if (OptionalRelevantTags)
		{
			// Ensure the global missing tag is only added once
			if (!bMissing)
			{
				UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
				const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;
				OptionalRelevantTags->AddTag(MissingTag);
			}

			FGameplayTagContainer MissingTags = RequiredTags;
			MissingTags.RemoveTags(TagsToCheck.GetGameplayTagParents());
			OptionalRelevantTags->AppendTags(MissingTags);
		}

		bMissing = true;
	};

	// check blocked first (so OptionalRelevantTags will contain blocked tags first)
	CheckForBlocked(AbilitySystemComponent.GetOwnedGameplayTags(), AdditionalBlockedTags);
	CheckForRequired(AbilitySystemComponent.GetOwnedGameplayTags(), AdditionalRequiredTags);

	return !bBlocked && !bMissing;
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

void UExtendedGameplayAbility::SetAbilityStateTags(const FGameplayTagContainer NewStateTags)
{
	AbilityStateTags = NewStateTags;
}

void UExtendedGameplayAbility::AddAbilityStateTag(const FGameplayTag StateTag)
{
	AbilityStateTags.AddTag(StateTag);
}

void UExtendedGameplayAbility::RemoveAbilityStateTag(const FGameplayTag StateTag)
{
	AbilityStateTags.RemoveTag(StateTag);
}

void UExtendedGameplayAbility::ClearAbilityStateTags()
{
	AbilityStateTags.Reset();
}

void UExtendedGameplayAbility::AddInputMappingContext(const UInputMappingContext* MappingContext, int32 Priority, const FModifyContextOptions& Options)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (const APlayerController* PC = GetPlayerControllerFromActorInfo())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			ActiveMappingContexts.AddUnique(MappingContext);
			Subsystem->AddMappingContext(MappingContext, Priority, Options);
		}
	}
}

void UExtendedGameplayAbility::RemoveInputMappingContext(const UInputMappingContext* MappingContext, const FModifyContextOptions& Options)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (const APlayerController* PC = GetPlayerControllerFromActorInfo())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			ActiveMappingContexts.Remove(MappingContext);
			Subsystem->RemoveMappingContext(MappingContext, Options);
		}
	}
}
