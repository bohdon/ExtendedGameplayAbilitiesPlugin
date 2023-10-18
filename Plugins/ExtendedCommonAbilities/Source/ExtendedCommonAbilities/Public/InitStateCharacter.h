// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "InitStateCharacter.generated.h"

class UPawnInitStateComponent;


UCLASS()
class EXTENDEDCOMMONABILITIES_API AInitStateCharacter : public AModularCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = "Character|InitState")
	TObjectPtr<UPawnInitStateComponent> InitStateComponent;

public:
	AInitStateCharacter(const FObjectInitializer& ObjectInitializer);

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
