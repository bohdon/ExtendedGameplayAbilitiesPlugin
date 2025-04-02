// Copyright Bohdon Sayre, All Rights Reserved.


#include "SampleGameplayStatics.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"


APlayerController* USampleGameplayStatics::GetPlayerControllerFromLocalPlayer(const UObject* WorldContextObject, const ULocalPlayer* LocalPlayer)
{
	if (!LocalPlayer)
	{
		return nullptr;
	}
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return LocalPlayer->GetPlayerController(World);
	}
	return nullptr;
}
