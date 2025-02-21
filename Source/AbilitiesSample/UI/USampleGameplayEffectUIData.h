// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData_TextOnly.h"
#include "USampleGameplayEffectUIData.generated.h"


/**
 * Gameplay effect UI data for the project.
 */
UCLASS(DisplayName="UI Data (Abilities Sample)")
class ABILITIESSAMPLE_API UUSampleGameplayEffectUIData : public UGameplayEffectUIData_TextOnly
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	FLinearColor Color = FLinearColor::White;
};
