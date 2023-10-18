// Copyright Bohdon Sayre, All Rights Reserved.


#include "InitStateCharacter.h"

#include "PawnInitStateComponent.h"


FName AInitStateCharacter::InitStateComponentName(TEXT("InitStateComponent"));

AInitStateCharacter::AInitStateCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitStateComponent = CreateDefaultSubobject<UPawnInitStateComponent>(InitStateComponentName);
}

void AInitStateCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitStateComponent->NotifyControllerChanged();
}

void AInitStateCharacter::UnPossessed()
{
	Super::UnPossessed();

	InitStateComponent->NotifyControllerChanged();
}

void AInitStateCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	InitStateComponent->NotifyControllerChanged();
}

void AInitStateCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitStateComponent->NotifyPlayerStateReplicated();
}

void AInitStateCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InitStateComponent->NotifySetupPlayerInputComponent(PlayerInputComponent);
}
