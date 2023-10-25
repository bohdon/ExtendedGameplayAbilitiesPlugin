// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedGameplayCueNotify_Looping.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemStats.h"
#include "ExtendedAbilitySystemStatics.h"
#include "Stats/Stats.h"
#include "Stats/Stats2.h"


AExtendedGameplayCueNotify_Looping::AExtendedGameplayCueNotify_Looping()
{
}

TArray<FActiveGameplayEffectHandle> AExtendedGameplayCueNotify_Looping::GetGrantingActiveEffects() const
{
	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	return UExtendedAbilitySystemStatics::GetActiveEffectsGrantingGameplayCue(AbilitySystem, GameplayCueTag);
}

void AExtendedGameplayCueNotify_Looping::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	// TODO: find out why linker error occurs when this is enabled
	// SCOPE_CYCLE_COUNTER(STAT_HandleGameplayCueNotifyActor);

	if (Parameters.MatchedTagName.IsValid() == false)
	{
		ABILITY_LOG(Warning, TEXT("GameplayCue parameter is none for %s"), *GetNameSafe(this));
	}

	// Handle multiple event gating
	{
		if (EventType == EGameplayCueEvent::OnActive && !bAllowMultipleOnActiveEvents && bHasHandledOnActiveEvent)
		{
			return;
		}

		if (EventType == EGameplayCueEvent::WhileActive && !bAllowMultipleWhileActiveEvents && bHasHandledWhileActiveEvent)
		{
			ABILITY_LOG(Log, TEXT("GameplayCue Notify %s WhileActive already handled, skipping this one."), *GetName());
			return;
		}

		if (EventType == EGameplayCueEvent::Removed && bHasHandledOnRemoveEvent)
		{
			return;
		}
	}

	// If cvar is enabled, check that the target no longer has the matched tag before doing remove logic. This is a simple way of supporting stacking,
	// such that if an actor has two sources giving it the same GC tag, it will not be removed when the first one is removed.
	const bool bShouldWaitForTagRemoval = !bUniqueInstancePerInstigator && !bUniqueInstancePerSourceObject; /*&& GameplayCueNotifyTagCheckOnRemove*/
	if (bShouldWaitForTagRemoval && EventType == EGameplayCueEvent::Removed)
	{
		if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MyTarget))
		{
			// FIX: The default implementation checks if the tag exists at all, which doesn't work since removal happens after this.
			//		Instead, skip if there is more than 1 count of the tag remaining.
			if (AbilitySystem->GetTagCount(Parameters.MatchedTagName) > 1)
			{
				return;
			}
		}
	}

	if (IsValid(MyTarget))
	{
		K2_HandleGameplayCue(MyTarget, EventType, Parameters);

		// Clear any pending auto-destroy that may have occurred from a previous OnRemove
		SetLifeSpan(0.f);

		switch (EventType)
		{
		case EGameplayCueEvent::OnActive:
			OnActive(MyTarget, Parameters);
			bHasHandledOnActiveEvent = true;
			break;

		case EGameplayCueEvent::WhileActive:
			WhileActive(MyTarget, Parameters);
			bHasHandledWhileActiveEvent = true;
			break;

		case EGameplayCueEvent::Executed:
			OnExecute(MyTarget, Parameters);
			break;

		case EGameplayCueEvent::Removed:
			bHasHandledOnRemoveEvent = true;
			OnRemove(MyTarget, Parameters);

			if (bAutoDestroyOnRemove)
			{
				if (AutoDestroyDelay > 0.f)
				{
					FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AGameplayCueNotify_Actor::GameplayCueFinishedCallback);
					GetWorld()->GetTimerManager().SetTimer(FinishTimerHandle, Delegate, AutoDestroyDelay, false);
				}
				else
				{
					GameplayCueFinishedCallback();
				}
			}
			break;
		};
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("Null Target called for event %d on GameplayCueNotifyActor %s"), (int32)EventType, *GetName());
		if (EventType == EGameplayCueEvent::Removed)
		{
			// Make sure the removed event is handled so that we don't leak GC notify actors
			GameplayCueFinishedCallback();
		}
	}
}

bool AExtendedGameplayCueNotify_Looping::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// AGameplayCueNotify_Actor does this, but Looping doesn't call the super implementation.
	// ReuseAfterRecycle also does this, but since the first time an actor is spawned it isn't recycled, so we need to do it here.
	if (IsHidden())
	{
		SetActorHiddenInGame(false);
	}

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}
