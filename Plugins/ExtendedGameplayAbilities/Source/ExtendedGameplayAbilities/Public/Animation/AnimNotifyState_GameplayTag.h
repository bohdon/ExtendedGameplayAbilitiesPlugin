// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_GameplayTag.generated.h"


/**
 * Applies a gameplay tag for the duration of an anim notify.
 */
UCLASS(Const, HideCategories = Object, CollapseCategories, Meta = (DisplayName = "Gameplay Tag"))
class EXTENDEDGAMEPLAYABILITIES_API UAnimNotifyState_GameplayTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_GameplayTag();

	/** The tag to add */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "AnimNotify")
	FGameplayTag GameplayTag;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};
