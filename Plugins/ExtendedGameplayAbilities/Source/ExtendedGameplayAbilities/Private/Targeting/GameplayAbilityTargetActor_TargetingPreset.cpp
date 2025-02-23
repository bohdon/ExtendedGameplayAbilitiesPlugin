// Copyright Bohdon Sayre, All Rights Reserved.


#include "Targeting/GameplayAbilityTargetActor_TargetingPreset.h"

#include "Abilities/GameplayAbility.h"
#include "Engine/GameInstance.h"
#include "Engine/NetSerialization.h"
#include "Engine/World.h"
#include "TargetingSystem/TargetingSubsystem.h"


AGameplayAbilityTargetActor_TargetingPreset::AGameplayAbilityTargetActor_TargetingPreset()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AGameplayAbilityTargetActor_TargetingPreset::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	if (!Preset)
	{
		CancelTargeting();
		return;
	}

	if (bContinuousTargeting && !bAsync)
	{
		// non-async continuous targeting must be performed manually every tick
		SetActorTickEnabled(true);
	}

	SpawnReticleActor();

	PerformTargetingInternal(bAsync);
}

void AGameplayAbilityTargetActor_TargetingPreset::SpawnReticleActor()
{
	if (!ReticleClass)
	{
		return;
	}

	AGameplayAbilityWorldReticle* NewReticle = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, GetActorLocation(), GetActorRotation());
	if (!NewReticle)
	{
		return;
	}

	ReticleActor = NewReticle;
	ReticleActor->InitializeReticle(this, PrimaryPC, ReticleParams);

	if (!ShouldProduceTargetDataOnServer)
	{
		ReticleActor->SetReplicates(false);
	}
}

bool AGameplayAbilityTargetActor_TargetingPreset::IsConfirmTargetingAllowed()
{
	return bHasTargetData;
}

void AGameplayAbilityTargetActor_TargetingPreset::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		if (bHasTargetData)
		{
			// use the cached target data from the latest request
			TargetDataReadyDelegate.Broadcast(TargetData);
		}
		else
		{
			// perform an immediate targeting request
			PerformTargetingInternal(false);
			if (bHasTargetData)
			{
				TargetDataReadyDelegate.Broadcast(TargetData);
			}
		}
	}
}

void AGameplayAbilityTargetActor_TargetingPreset::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (OwningAbility && bContinuousTargeting && !bAsync && !bIsRequestInProgress)
	{
		PerformTargetingInternal(bAsync);
	}
}

void AGameplayAbilityTargetActor_TargetingPreset::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ReticleActor.IsValid())
	{
		ReticleActor.Get()->Destroy();
	}
	if (EndPlayReason != EEndPlayReason::EndPlayInEditor && EndPlayReason != EEndPlayReason::Quit)
	{
		if (TargetingHandle.IsValid())
		{
			UTargetingSubsystem::ReleaseTargetRequestHandle(TargetingHandle);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AGameplayAbilityTargetActor_TargetingPreset::PerformTargeting()
{
	if (!bIsRequestInProgress)
	{
		PerformTargetingInternal(bAsync);
	}
}

void AGameplayAbilityTargetActor_TargetingPreset::PerformTargetingInternal(bool bInAsync)
{
	check(Preset);

	UTargetingSubsystem* TargetingSubsystem = GetGameInstance()->GetSubsystem<UTargetingSubsystem>();

	if (bIsRequestInProgress)
	{
		if (bAsync)
		{
			// async request already in progress
			return;
		}

		// cancel any async requests that might be active
		TargetingSubsystem->RemoveAsyncTargetingRequestWithHandle(TargetingHandle);
		bIsRequestInProgress = false;
	}

	// setup request handle and data stores that will be re-used
	if (!TargetingHandle.IsValid())
	{
		TargetingHandle = UTargetingSubsystem::MakeTargetRequestHandle(Preset, GetTargetingContext());
	}
	else
	{
		// ensure previous results are cleared before every update
		// (continuous async targeting handles this automatically with bRequeueOnCompletion)
		if (FTargetingDefaultResultsSet* Results = FTargetingDefaultResultsSet::Find(TargetingHandle))
		{
			Results->TargetResults.Empty();
		}
	}

	const FTargetingRequestDelegate Delegate = FTargetingRequestDelegate::CreateUObject(
		this, &AGameplayAbilityTargetActor_TargetingPreset::OnTargetingRequestCompleted);

	bIsRequestInProgress = true;
	if (bInAsync)
	{
		if (bContinuousTargeting)
		{
			// continuous async targeting is done automatically just by enabling bRequeueOnCompletion
			FTargetingAsyncTaskData& AsyncTaskData = FTargetingAsyncTaskData::FindOrAdd(TargetingHandle);
			AsyncTaskData.bRequeueOnCompletion = true;
		}

		TargetingSubsystem->StartAsyncTargetingRequestWithHandle(TargetingHandle, Delegate);
	}
	else
	{
		// completion delegate will be called immediately
		TargetingSubsystem->ExecuteTargetingRequestWithHandle(TargetingHandle, Delegate);
	}
}

void AGameplayAbilityTargetActor_TargetingPreset::OnTargetingRequestCompleted(FTargetingRequestHandle TargetingRequestHandle)
{
	// continuous async is 'always' in progress via bRequeueOnCompletion
	if (!(bContinuousTargeting && bAsync))
	{
		bIsRequestInProgress = false;
	}

	UpdateTargetData();
}

FTargetingSourceContext AGameplayAbilityTargetActor_TargetingPreset::GetTargetingContext()
{
	FTargetingSourceContext Context;
	Context.SourceActor = SourceActor;
	Context.InstigatorActor = SourceActor;
	Context.SourceLocation = StartLocation.GetTargetingTransform().GetLocation();
	Context.SourceSocketName = StartLocation.SourceSocketName;
	Context.SourceObject = this;
	return Context;
}

void AGameplayAbilityTargetActor_TargetingPreset::UpdateTargetData()
{
	if (!TargetingHandle.IsValid())
	{
		// targeting request isn't ready
		return;
	}

	const FTargetingDefaultResultsSet* Results = FTargetingDefaultResultsSet::Find(TargetingHandle);
	if (!Results)
	{
		return;
	}

	bHasTargetData = true;

	TargetData.Clear();
	TOptional<FHitResult> FirstHit;

	for (const FTargetingDefaultResultData& Result : Results->TargetResults)
	{
		/** Note: These are cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
		FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
		ReturnData->HitResult = Result.HitResult;
		TargetData.Add(ReturnData);

		if (!FirstHit.IsSet())
		{
			FirstHit = Result.HitResult;
		}
	}

	// update reticle
	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		if (FirstHit.IsSet())
		{
			const FHitResult& Hit = FirstHit.GetValue();
			const bool bHitActor = Hit.bBlockingHit && Hit.HitObjectHandle.IsValid();
			const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor)
				                                ? Hit.HitObjectHandle.GetLocation()
				                                : FVector(Hit.Location);

			LocalReticleActor->SetActorLocation(ReticleLocation);
			LocalReticleActor->SetIsTargetAnActor(bHitActor);
		}
		else
		{
			LocalReticleActor->SetIsTargetAnActor(false);
		}
	}
}
