// Copyright Bohdon Sayre, All Rights Reserved.


#include "Teams/CommonTeamStatics.h"

#include "Engine/Engine.h"
#include "GameFramework/GameStateBase.h"
#include "Teams/CommonTeamsComponent.h"


UCommonTeamsComponent* UCommonTeamStatics::GetTeamsComponent(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->FindComponentByClass<UCommonTeamsComponent>();
		}
	}
	return nullptr;
}

void UCommonTeamStatics::CompareTeams(const UObject* ObjectA, const UObject* ObjectB, TEnumAsByte<ETeamAttitude::Type>& Attitude, ECommonTeamComparison& Comparison)
{
	if (const UCommonTeamsComponent* TeamsComp = GetTeamsComponent(ObjectA))
	{
		Attitude = TeamsComp->GetAttitude(ObjectA, ObjectB);
		Comparison = TeamsComp->CompareTeams(ObjectA, ObjectB);
	}
	else
	{
		Attitude = ETeamAttitude::Neutral;
		Comparison = ECommonTeamComparison::NoTeam;
	}
}
