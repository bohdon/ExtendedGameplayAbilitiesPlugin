// Copyright Bohdon Sayre, All Rights Reserved.


#include "Health/CommonHealthComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameplayEffectExtension.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Death, "Event.Death");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Death_SelfDestruct, "Event.Death.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Death_Dying, "State.Death.Dying");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Death_Dead, "State.Death.Dead");


UCommonHealthComponent::UCommonHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bAutoRegisterAbilitySystem(true),
	  HealthState(ECommonHealthState::Alive),
	  AbilitySystem(nullptr)
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
}

void UCommonHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCommonHealthComponent, HealthState);
}

void UCommonHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (bAutoRegisterAbilitySystem && GetOwner())
	{
		SetAbilitySystem(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
	}
}

void UCommonHealthComponent::OnUnregister()
{
	Super::OnUnregister();

	SetAbilitySystem(nullptr);
}

void UCommonHealthComponent::ClearGameplayTags()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(TAG_State_Death_Dying, 0);
		AbilitySystem->SetLooseGameplayTagCount(TAG_State_Death_Dead, 0);
	}
}

void UCommonHealthComponent::SetAbilitySystem(UAbilitySystemComponent* InAbilitySystem)
{
	if (AbilitySystem != InAbilitySystem)
	{
		if (AbilitySystem)
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttribute).RemoveAll(this);
			ClearGameplayTags();
		}

		AbilitySystem = InAbilitySystem;

		if (AbilitySystem)
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddUObject(this, &UCommonHealthComponent::OnHPChanged);
		}
	}
}

void UCommonHealthComponent::TriggerDeathFromSelfDestruct()
{
	TriggerDeath(GetOwner(), FGameplayEffectContextHandle(), TAG_Event_Death_SelfDestruct);
}

void UCommonHealthComponent::TriggerDeath(AActor* Instigator, FGameplayEffectContextHandle Context, FGameplayTag DeathEventTag)
{
	if (!DeathEventTag.MatchesTag(TAG_Event_Death))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("DeathEventTag must be Event.Death or a child tag."));
		return;
	}

	// send Event.Death to trigger death ability which calls StartDeath and FinishDeath
	FGameplayEventData Payload;
	Payload.Target = AbilitySystem->GetAvatarActor();
	Payload.EventTag = DeathEventTag;
	Payload.Instigator = Instigator;
	Payload.ContextHandle = Context;
	AbilitySystem->GetOwnedGameplayTags(Payload.TargetTags);

	FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
	AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void UCommonHealthComponent::StartDeath()
{
	if (HealthState == ECommonHealthState::Alive)
	{
		HealthState = ECommonHealthState::DeathStarted;
		OnDeathStarted();
	}
}

void UCommonHealthComponent::FinishDeath()
{
	if (HealthState == ECommonHealthState::DeathStarted)
	{
		HealthState = ECommonHealthState::DeathFinished;
		OnDeathFinished();
	}
}

void UCommonHealthComponent::OnDeathStarted()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(TAG_State_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	Owner->ForceNetUpdate();

	OnDeathStartedEvent.Broadcast(Owner);
	OnDeathStartedEvent_BP.Broadcast(Owner);
}

void UCommonHealthComponent::OnDeathFinished()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(TAG_State_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	Owner->ForceNetUpdate();

	OnDeathFinishedEvent.Broadcast(Owner);
	OnDeathFinishedEvent_BP.Broadcast(Owner);
}

void UCommonHealthComponent::OnRep_HealthState(ECommonHealthState OldHealthState)
{
	if (OldHealthState == ECommonHealthState::Alive)
	{
		if (HealthState == ECommonHealthState::DeathStarted)
		{
			OnDeathStarted();
		}
		else if (HealthState == ECommonHealthState::DeathFinished)
		{
			OnDeathStarted();
			OnDeathFinished();
		}
	}
	else if (OldHealthState == ECommonHealthState::DeathStarted)
	{
		if (HealthState == ECommonHealthState::DeathFinished)
		{
			OnDeathFinished();
		}
	}
}

void UCommonHealthComponent::OnHPChanged(const FOnAttributeChangeData& ChangeData)
{
#if WITH_SERVER_CODE
	if (ChangeData.OldValue > 0 && ChangeData.NewValue <= 0)
	{
		{
			// send Event.Death to trigger death ability which calls StartDeath and FinishDeath
			FGameplayEventData Payload;
			Payload.Target = AbilitySystem->GetAvatarActor();
			Payload.EventTag = TAG_Event_Death;
			if (ChangeData.GEModData)
			{
				Payload.ContextHandle = ChangeData.GEModData->EffectSpec.GetEffectContext();
				Payload.Instigator = Payload.ContextHandle.GetOriginalInstigator();
				Payload.OptionalObject = ChangeData.GEModData->EffectSpec.Def;
				Payload.InstigatorTags = *ChangeData.GEModData->EffectSpec.CapturedSourceTags.GetAggregatedTags();
				Payload.TargetTags = *ChangeData.GEModData->EffectSpec.CapturedTargetTags.GetAggregatedTags();
				Payload.EventMagnitude = ChangeData.GEModData->EvaluatedData.Magnitude;
			}

			FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
			AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}
#endif
}
