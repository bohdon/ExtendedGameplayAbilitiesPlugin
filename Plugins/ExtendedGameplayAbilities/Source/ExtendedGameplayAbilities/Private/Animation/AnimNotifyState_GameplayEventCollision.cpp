// Copyright Bohdon Sayre, All Rights Reserved.


#include "Animation/AnimNotifyState_GameplayEventCollision.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"


void UAnimNotifyState_GameplayEventCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                                          const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UPrimitiveComponent* CollisionComp = SpawnCollision(MeshComp, Animation))
	{
		// tag the component so it can be cleaned up
		CollisionComp->ComponentTags.AddUnique(GetSpawnedComponentTag());
	}
}

void UAnimNotifyState_GameplayEventCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                        const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UPrimitiveComponent* PrimitiveComp = GetSpawnedCollision(MeshComp))
	{
		PrimitiveComp->DestroyComponent();
	}
}

FString UAnimNotifyState_GameplayEventCollision::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("%s Collision -> %s"),
	                       *StaticEnum<EGameplayEventCollisionShapeType>()->GetNameStringByValue(static_cast<int64>(ShapeType)),
	                       *BeginOverlapEventTag.ToString());
}

float UAnimNotifyState_GameplayEventCollision::GetShapeRadius() const
{
	return DefaultRadius;
}

float UAnimNotifyState_GameplayEventCollision::GetShapeCapsuleHalfHeight() const
{
	return DefaultCapsuleHalfHeight;
}

FVector UAnimNotifyState_GameplayEventCollision::GetShapeBoxHalfExtents() const
{
	return DefaultBoxHalfExtents;
}

UPrimitiveComponent* UAnimNotifyState_GameplayEventCollision::SpawnCollision(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	TSubclassOf<UPrimitiveComponent> ComponentClass;
	switch (ShapeType)
	{
	case EGameplayEventCollisionShapeType::Sphere:
		ComponentClass = USphereComponent::StaticClass();
		break;
	case EGameplayEventCollisionShapeType::Capsule:
		ComponentClass = UCapsuleComponent::StaticClass();
		break;
	case EGameplayEventCollisionShapeType::Box:
		ComponentClass = UBoxComponent::StaticClass();
		break;
	}

	if (UPrimitiveComponent* CollisionComp = NewObject<UPrimitiveComponent>(MeshComp, ComponentClass, NAME_None, RF_Transient))
	{
		CollisionComp->SetCollisionProfileName(CollisionProfileName.Name);
		CollisionComp->SetupAttachment(MeshComp, SocketName);
		CollisionComp->SetRelativeLocationAndRotation(Location, Rotation);

		switch (ShapeType)
		{
		case EGameplayEventCollisionShapeType::Sphere:
			if (USphereComponent* SphereComp = Cast<USphereComponent>(CollisionComp))
			{
				SphereComp->SetSphereRadius(GetShapeRadius(), false);
			}
			break;
		case EGameplayEventCollisionShapeType::Capsule:
			if (UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(CollisionComp))
			{
				CapsuleComp->SetCapsuleSize(GetShapeRadius(), GetShapeCapsuleHalfHeight(), false);
			}
			break;
		case EGameplayEventCollisionShapeType::Box:
			if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(CollisionComp))
			{
				BoxComponent->SetBoxExtent(GetShapeBoxHalfExtents(), false);
			}
			break;
		}

		if (BeginOverlapEventTag.IsValid())
		{
			CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &UAnimNotifyState_GameplayEventCollision::OnBeginOverlap);
		}
		if (EndOverlapEventTag.IsValid())
		{
			CollisionComp->OnComponentEndOverlap.AddDynamic(this, &UAnimNotifyState_GameplayEventCollision::OnEndOverlap);
		}

		CollisionComp->RegisterComponent();

		return CollisionComp;
	}
	return nullptr;
}

UPrimitiveComponent* UAnimNotifyState_GameplayEventCollision::GetSpawnedCollision(UMeshComponent* MeshComp) const
{
	if (!MeshComp)
	{
		return nullptr;
	}

	TArray<USceneComponent*> Children;
	MeshComp->GetChildrenComponents(false, Children);

	const FName ComponentTag = GetSpawnedComponentTag();
	for (USceneComponent* Component : Children)
	{
		if (Component && Component->ComponentHasTag(ComponentTag))
		{
			if (UPrimitiveComponent* PrimitiveComp = CastChecked<UPrimitiveComponent>(Component))
			{
				return PrimitiveComp;
			}
		}
	}
	return nullptr;
}

bool UAnimNotifyState_GameplayEventCollision::ShouldIgnoreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) const
{
	if (bIgnoreSelf && OtherActor == OverlappedComponent->GetOwner())
	{
		return true;
	}
	return false;
}

void UAnimNotifyState_GameplayEventCollision::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ShouldIgnoreOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex))
	{
		return;
	}

	AActor* OwningActor = OverlappedComponent->GetOwner();
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
	{
		FGameplayEventData Payload;
		Payload.EventTag = BeginOverlapEventTag;
		Payload.Instigator = OwningActor;
		Payload.Target = AbilitySystem->GetAvatarActor();

		// create target data from the hit result
		/** Note: These are cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
		FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
		ReturnData->HitResult = SweepResult;
		Payload.TargetData.Add(ReturnData);

		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void UAnimNotifyState_GameplayEventCollision::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                           int32 OtherBodyIndex)
{
	if (ShouldIgnoreOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex))
	{
		return;
	}

	AActor* OwningActor = OverlappedComponent->GetOwner();
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
	{
		FGameplayEventData Payload;
		Payload.EventTag = EndOverlapEventTag;
		Payload.Instigator = OwningActor;
		Payload.Target = AbilitySystem->GetAvatarActor();

		// make a fake hit result for the target data
		FHitResult HitResult;
		HitResult.HitObjectHandle = FActorInstanceHandle(OtherActor);
		HitResult.Component = OtherComp;

		// create target data from the hit result
		/** Note: These are cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
		FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
		ReturnData->HitResult = HitResult;
		Payload.TargetData.Add(ReturnData);

		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

#if WITH_EDITOR
bool UAnimNotifyState_GameplayEventCollision::CanEditChange(const FProperty* InProperty) const
{
	const bool bCanEdit = Super::CanEditChange(InProperty);

	if (bCanEdit && InProperty)
	{
		const FName PropertyName = InProperty->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_GameplayEventCollision, DefaultRadius))
		{
			return ShapeType == EGameplayEventCollisionShapeType::Sphere || ShapeType == EGameplayEventCollisionShapeType::Capsule;
		}

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_GameplayEventCollision, DefaultCapsuleHalfHeight))
		{
			return ShapeType == EGameplayEventCollisionShapeType::Capsule;
		}

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_GameplayEventCollision, DefaultBoxHalfExtents))
		{
			return ShapeType == EGameplayEventCollisionShapeType::Box;
		}
	}

	return true;
}
#endif
