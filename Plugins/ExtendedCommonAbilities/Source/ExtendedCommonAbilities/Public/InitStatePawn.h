// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "InitStatePawn.generated.h"

class UPawnInitStateComponent;

UCLASS()
class EXTENDEDCOMMONABILITIES_API AInitStatePawn : public AModularPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = "Character|InitState")
	TObjectPtr<UPawnInitStateComponent> InitStateComponent;

public:
	AInitStatePawn(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	/** Return the InitStateComponent subobject. */
	UPawnInitStateComponent* GetInitStateComponent() const { return InitStateComponent; }

public:
	static FName InitStateComponentName;
};
