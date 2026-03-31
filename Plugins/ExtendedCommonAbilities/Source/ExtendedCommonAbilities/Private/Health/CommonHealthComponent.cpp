// Copyright Bohdon Sayre, All Rights Reserved.


#include "Health/CommonHealthComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "AsyncMessageSystemBase.h"
#include "AsyncMessageWorldSubsystem.h"
#include "ExtendedCommonAbilitiesTags.h"
#include "GameplayEffectExtension.h"
#include "HPAttributeSet.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "StructUtils/InstancedStruct.h"


UCommonHealthComponent::UCommonHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bAutoRegisterAbilitySystem(true),
	  bSendGameplayMessage(true),
	  GameplayMessageChannel(ExtendedCommonAbilities::GameplayTags::GameplayMessage_Death),
	  HealthState(ECommonHealthState::Alive),
	  AbilitySystem(nullptr)
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	HealthAttribute = UHPAttributeSet::GetHPAttribute();

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
		AbilitySystem->SetLooseGameplayTagCount(ExtendedCommonAbilities::GameplayTags::State_Death_Dying, 0);
		AbilitySystem->SetLooseGameplayTagCount(ExtendedCommonAbilities::GameplayTags::State_Death_Dead, 0);
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

void UCommonHealthComponent::ClearAbilitySystem()
{
	SetAbilitySystem(nullptr);
}

void UCommonHealthComponent::TriggerDeathFromSelfDestruct()
{
	TriggerDeath(GetOwner(), FGameplayEffectContextHandle(), ExtendedCommonAbilities::GameplayTags::Event_Death_SelfDestruct);
}

void UCommonHealthComponent::TriggerDeath(AActor* Instigator, FGameplayEffectContextHandle Context, FGameplayTag DeathEventTag)
{
	if (!DeathEventTag.MatchesTag(ExtendedCommonAbilities::GameplayTags::Event_Death))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[%s] TriggerDeath: DeathEventTag must be %s or a child tag."),
		       *GetReadableName(), *ExtendedCommonAbilities::GameplayTags::Event_Death.GetTag().ToString());
		return;
	}

	FGameplayEventData EventData;
	EventData.Target = AbilitySystem->GetAvatarActor();
	EventData.EventTag = DeathEventTag;
	EventData.Instigator = Instigator;
	EventData.ContextHandle = Context;
	AbilitySystem->GetOwnedGameplayTags(EventData.TargetTags);

	{
		// send Event.Death to trigger death ability which calls StartDeath and FinishDeath
		const FGameplayEventData Payload = EventData;

		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
	}

	if (bSendGameplayMessage)
	{
		if (const TSharedPtr<FAsyncMessageSystemBase> Sys = UAsyncMessageWorldSubsystem::GetSharedMessageSystem(GetWorld()))
		{
			// broadcast event to everyone
			FGameplayEventData Message = EventData;
			Message.EventTag = GameplayMessageChannel;

			const FInstancedStruct Payload = FInstancedStruct::Make<FGameplayEventData>(MoveTemp(Message));
			Sys->QueueMessageForBroadcast(Message.EventTag, Payload);
		}
	}
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
		AbilitySystem->SetLooseGameplayTagCount(ExtendedCommonAbilities::GameplayTags::State_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	Owner->ForceNetUpdate();

	OnDeathStartedEvent.Broadcast();
	OnDeathStartedEvent_BP.Broadcast(Owner);
}

void UCommonHealthComponent::OnDeathFinished()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(ExtendedCommonAbilities::GameplayTags::State_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	Owner->ForceNetUpdate();

	OnDeathFinishedEvent.Broadcast();
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
	if (ChangeData.OldValue > 0 && ChangeData.NewValue <= 0)
	{
		FGameplayEventData EventData;
		EventData.Target = AbilitySystem->GetAvatarActor();
		EventData.EventTag = ExtendedCommonAbilities::GameplayTags::Event_Death;
		if (ChangeData.GEModData)
		{
			EventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetEffectContext();
			EventData.Instigator = EventData.ContextHandle.GetOriginalInstigator();
			EventData.OptionalObject = ChangeData.GEModData->EffectSpec.Def;
			EventData.InstigatorTags = *ChangeData.GEModData->EffectSpec.CapturedSourceTags.GetAggregatedTags();
			EventData.TargetTags = *ChangeData.GEModData->EffectSpec.CapturedTargetTags.GetAggregatedTags();
			EventData.EventMagnitude = ChangeData.GEModData->EvaluatedData.Magnitude;
		}

		{
			// send Event.Death to trigger death ability which calls StartDeath and FinishDeath
			const FGameplayEventData Payload = EventData;

			FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
			AbilitySystem->HandleGameplayEvent(EventData.EventTag, &Payload);
		}

		if (bSendGameplayMessage)
		{
			if (const TSharedPtr<FAsyncMessageSystemBase> Sys = UAsyncMessageWorldSubsystem::GetSharedMessageSystem(GetWorld()))
			{
				// broadcast event to everyone
				FGameplayEventData Message = EventData;
				Message.EventTag = GameplayMessageChannel;

				const FInstancedStruct Payload = FInstancedStruct::Make<FGameplayEventData>(MoveTemp(Message));
				Sys->QueueMessageForBroadcast(Message.EventTag, Payload);
			}
		}
	}
}
