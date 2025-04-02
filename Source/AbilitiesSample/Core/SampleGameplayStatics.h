// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SampleGameplayStatics.generated.h"

class ULocalPlayer;


/**
 * Some static functions for the sample project.
 */
UCLASS()
class ABILITIESSAMPLE_API USampleGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Return the PlayerController for a local player. */
	UFUNCTION(BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetPlayerControllerFromLocalPlayer(const UObject* WorldContextObject, const ULocalPlayer* LocalPlayer);
};
