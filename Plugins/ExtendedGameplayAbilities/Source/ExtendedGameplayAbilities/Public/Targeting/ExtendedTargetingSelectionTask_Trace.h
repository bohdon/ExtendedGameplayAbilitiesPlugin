// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "ScalableFloat.h"
#include "Engine/CollisionProfile.h"
#include "Tasks/TargetingSelectionTask_Trace.h"
#include "Tasks/TargetingTask.h"
#include "Types/TargetingSystemTypes.h"
#include "UObject/Object.h"

#include "ExtendedTargetingSelectionTask_Trace.generated.h"


class UTargetingSubsystem;
struct FCollisionQueryParams;
struct FTargetingDebugInfo;
struct FTargetingDefaultResultData;
struct FTargetingRequestHandle;
struct FTraceDatum;
struct FTraceHandle;


UENUM(BlueprintType)
enum class EExtendedTargetingTraceSourceLocation : uint8
{
	/** Use the SourceActor's location. */
	SourceActor,
	/** Use the camera location for locally controlled players, or fall back to the SourceActor location. */
	CameraOrSourceActor,
	/** Use the SourceLocation from the targeting context. */
	SourceLocation,
};


/**
 * An almost-exact copy of UTargetingSelectionTask_Trace, but with a few changes:
 * - Allow non-blocking hit results, so that empty traces can still return a result at max range
 * - Add control over whether swept shapes are rotated to align with the trace direction, or use absolute rotations
 * - Provide more default options for SourceLocation, instead of just SourceActor location
 * - Fix Async traces always using Multi mode, despite bMultiTrace
 *
 * Not a subclass since important functions are private or not virtual.
 */
UCLASS(Blueprintable)
class EXTENDEDGAMEPLAYABILITIES_API UExtendedTargetingSelectionTask_Trace : public UTargetingTask
{
	GENERATED_BODY()

public:
	UExtendedTargetingSelectionTask_Trace(const FObjectInitializer& ObjectInitializer);

	/** Evaluation function called by derived classes to process the targeting request */
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;

protected:
	/** Native Event to get the source location for the Trace */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	FVector GetSourceLocation(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get a source location offset for the Trace */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	FVector GetSourceOffset(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get the direction for the Trace */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	FVector GetTraceDirection(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get the length for the Trace */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	float GetTraceLength(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get the swept trace radius (only called if TraceType = ETargetingTraceType::Sphere or TraceType = ETargetingTraceType::Capsule) */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	float GetSweptTraceRadius(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get the swept trace capsule's half height (only called if  TraceType = ETargetingTraceType::Capsule) */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	float GetSweptTraceCapsuleHalfHeight(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get the swept box trace half extents (only called if TraceType = ETargetingTraceType::Box) */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	FVector GetSweptTraceBoxHalfExtents(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get the swept trace rotation relative to trace direction
	(only called if TraceType = ETargetingTraceType::Capsule or TraceType = ETargetingTraceType::Box) */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	FRotator GetSweptTraceRotation(const FTargetingRequestHandle& TargetingHandle) const;

	/** Native Event to get additional actors the Trace should ignore */
	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
	void GetAdditionalActorsToIgnore(const FTargetingRequestHandle& TargetingHandle, TArray<AActor*>& OutAdditionalActorsToIgnore) const;

private:
	/** Method to process the trace task immediately */
	void ExecuteImmediateTrace(const FTargetingRequestHandle& TargetingHandle) const;

	/** Method to process the trace task asynchronously */
	void ExecuteAsyncTrace(const FTargetingRequestHandle& TargetingHandle) const;

	/** Callback for an async trace */
	void HandleAsyncTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum, FTargetingRequestHandle TargetingHandle) const;

	/** Method to take the hit results and store them in the targeting result data */
	void ProcessHitResults(const FTargetingRequestHandle& TargetingHandle, const TArray<FHitResult>& Hits) const;

	/** Setup CollisionQueryParams for the trace */
	void InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const;

	/** For non-sphere shape traces, calculates the world rotation for that trace. */
	FQuat GetSweptTraceQuat(const FVector& TraceDirection, const FTargetingRequestHandle& TargetingHandle) const;

protected:
	/** The trace type (i.e. shape) to use */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Collision Data")
	ETargetingTraceType TraceType = ETargetingTraceType::Line;

	/** The trace channel to use */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Collision Data")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = TraceTypeQuery1;

	/** The collision profile name to use instead of trace channel (does not work for async traces) */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Collision Data")
	FCollisionProfileName CollisionProfileName;

	/** The default swept trace radius used by GetSweptTraceRadius when the trace type is set to Sphere or Capsule */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	FScalableFloat DefaultSweptTraceRadius = 10.0f;

	/** The default swept trace radius used by GetSweptTraceRadius when the trace type is set to Capsule */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	FScalableFloat DefaultSweptTraceCapsuleHalfHeight = 10.0f;

	/** The default swept extents used by GetSweptTraceExtents when the trace type is set to Box */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	FScalableFloat DefaultSweptTraceBoxHalfExtentX = 10.0f;

	/** The default swept extents used by GetSweptTraceBoxHalfExtents when the trace type is set to Box */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	FScalableFloat DefaultSweptTraceBoxHalfExtentY = 10.0f;

	/** The default swept extents used by GetSweptTraceBoxHalfExtents when the trace type is set to Box  */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	FScalableFloat DefaultSweptTraceBoxHalfExtentZ = 10.0f;

	/** The default swept rotation (relative to the trace direction) used by GetSweptTraceRotation when the trace type is set to Capsule or Box */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	FRotator DefaultSweptTraceRotation = FRotator::ZeroRotator;

	/** If true, orient swept shapes like Capsule to be aligned with the trace direction, and apply DefaultSweptTraceRotation as a relative rotation. */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
	bool bOrientSweptShapesToDirection = true;

	/** The default trace length to use if GetTraceLength is not overridden by a child */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	FScalableFloat DefaultTraceLength = 10.0f;

	/** The default source location to use. */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	EExtendedTargetingTraceSourceLocation SourceLocationType = EExtendedTargetingTraceSourceLocation::SourceActor;

	/**
	 * When using CameraOrSourceActor, start the trace along the camera direction from a point closest to the source actor,
	 * to ignore any collision between the camera and that actor.
	 */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data",
		meta = (EditCondition = "SourceLocationType == EExtendedTargetingTraceSourceLocation::CameraOrSourceActor"))
	bool bStartCameraTraceCloseToSourceActor = true;

	/** The default source location offset used by GetSourceOffset */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	FVector DefaultSourceOffset = FVector::ZeroVector;

	/** An explicit trace direction to use (default uses pawn control rotation or actor forward vector in GetTraceDirection) */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	FVector ExplicitTraceDirection = FVector::ZeroVector;

	/** Indicates the trace should perform a complex trace */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	uint8 bComplexTrace : 1;

	/** Indicates whether the trace should be a multi trace or a single trace */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	uint8 bMultiTrace : 1;

	/** Indicates the trace should ignore the source actor */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	uint8 bIgnoreSourceActor : 1;

	/** Indicates the trace should ignore the source actor */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	uint8 bIgnoreInstigatorActor : 1;

	/**
	 * Include all hit results.
	 * When false, non-blocking hits will be excluded, as well as
	 * any hit results for actors that are already in the result data.
	 */
	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
	uint8 bIncludeAllHitResults : 1;

protected:
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

#if ENABLE_DRAW_DEBUG

private:
	virtual void DrawDebug(UTargetingSubsystem* TargetingSubsystem, FTargetingDebugInfo& Info, const FTargetingRequestHandle& TargetingHandle,
	                       float XOffset, float YOffset, int32 MinTextRowsToAdvance) const override;

	/** Draw debug info showing the results of the shape trace used for targeting. */
	virtual void DrawDebugTrace(const FTargetingRequestHandle TargetingHandle, const FVector& StartLocation, const FVector& EndLocation,
	                            const FQuat& OrientationQuat, const bool bHit, const TArray<FHitResult>& Hits) const;

	void BuildTraceResultsDebugString(const FTargetingRequestHandle& TargetingHandle, const TArray<FTargetingDefaultResultData>& TargetResults) const;
	void ResetTraceResultsDebugString(const FTargetingRequestHandle& TargetingHandle) const;
#endif
};
