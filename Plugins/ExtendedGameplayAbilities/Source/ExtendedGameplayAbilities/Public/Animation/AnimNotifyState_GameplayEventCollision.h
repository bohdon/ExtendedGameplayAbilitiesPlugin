// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Engine/CollisionProfile.h"
#include "Engine/HitResult.h"
#include "AnimNotifyState_GameplayEventCollision.generated.h"

class UMeshComponent;
class UPrimitiveComponent;
class USkeletalMeshComponent;


UENUM(BlueprintType)
enum class EGameplayEventCollisionShapeType : uint8
{
	Sphere,
	Capsule,
	Box,
};


/**
 * Adds a collision component to a bone or socket, and triggers
 * a gameplay event when on overlap.
 */
UCLASS(Const, HideCategories = Object, CollapseCategories, Meta = (DisplayName = "Gameplay Event Collision"))
class EXTENDEDGAMEPLAYABILITIES_API UAnimNotifyState_GameplayEventCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** The gameplay event tag to send on overlap begin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FGameplayTag BeginOverlapEventTag;

	/** The gameplay event tag to send on overlap end. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FGameplayTag EndOverlapEventTag;

	/** The type of shape to create. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	EGameplayEventCollisionShapeType ShapeType;

	/** The collision profile name to use for the shape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FCollisionProfileName CollisionProfileName;

	/** The socket or bone to attach to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FName SocketName;

	/** The relative location of the collision shape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FVector Location;

	/** The relative rotation of the collision shape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FRotator Rotation;

	/** The default shape radius when the shape is set to Sphere or Capsule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float DefaultRadius = 10.0f;

	/** The default capsule half height when the shape is set to Capsule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float DefaultCapsuleHalfHeight = 10.0f;

	/** The default extents when the shape is set to Box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FVector DefaultBoxHalfExtents = FVector(10.0f, 10.f, 10.f);

	/** Ignore collisions with the owning actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bIgnoreSelf = true;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

protected:
	virtual float GetShapeRadius() const;
	virtual float GetShapeCapsuleHalfHeight() const;
	virtual FVector GetShapeBoxHalfExtents() const;

	/** Spawn the collision component. */
	virtual UPrimitiveComponent* SpawnCollision(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;

	/** Return the spawned collision component for a mesh. */
	virtual UPrimitiveComponent* GetSpawnedCollision(UMeshComponent* MeshComp) const;

	FORCEINLINE FName GetSpawnedComponentTag() const { return GetFName(); }

	virtual bool ShouldIgnoreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) const;
};
