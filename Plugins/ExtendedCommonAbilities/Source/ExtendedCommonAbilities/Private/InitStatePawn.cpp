// Copyright Bohdon Sayre, All Rights Reserved.


#include "InitStatePawn.h"

#include "PawnInitStateComponent.h"


FName AInitStatePawn::InitStateComponentName(TEXT("InitStateComponent"));

AInitStatePawn::AInitStatePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitStateComponent = CreateDefaultSubobject<UPawnInitStateComponent>(InitStateComponentName);
}

void AInitStatePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitStateComponent->NotifyControllerChanged();
}

void AInitStatePawn::UnPossessed()
{
	Super::UnPossessed();

	InitStateComponent->NotifyControllerChanged();
}

void AInitStatePawn::OnRep_Controller()
{
	Super::OnRep_Controller();

	InitStateComponent->NotifyControllerChanged();
}

void AInitStatePawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitStateComponent->NotifyPlayerStateReplicated();
}

void AInitStatePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InitStateComponent->NotifySetupPlayerInputComponent(PlayerInputComponent);
}
