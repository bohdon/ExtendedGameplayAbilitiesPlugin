// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonTeamDef.generated.h"


/**
 * The definition for a team and any displayable properties for it.
 * Subclass to add more info like color, icons, etc.
 */
UCLASS(BlueprintType, Blueprintable)
class EXTENDEDCOMMONABILITIES_API UCommonTeamDef : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The display name of the team */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
};
