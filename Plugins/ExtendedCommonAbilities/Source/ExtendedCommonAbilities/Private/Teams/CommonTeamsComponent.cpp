// Copyright Bohdon Sayre, All Rights Reserved.


#include "Teams/CommonTeamsComponent.h"

#include "GenericTeamAgentInterface.h"
#include "Framework/Commands/GenericCommands.h"
#include "GameFramework/PlayerState.h"
#include "Teams/CommonTeamStatics.h"


UCommonTeamsComponent::UCommonTeamsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UCommonTeamDef* UCommonTeamsComponent::GetTeamDefinition(int32 TeamId) const
{
	return TeamDefinitions.FindRef(TeamId);
}

void UCommonTeamsComponent::BeginPlay()
{
	Super::BeginPlay();

	StartTeamSetup();
}

void UCommonTeamsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
}

void UCommonTeamsComponent::StartTeamSetup()
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		// setup immediately, you may want to delay this until everything is ready in some situations
		ServerAssignTeamsForPlayers();
	}
#endif
}

int32 UCommonTeamsComponent::SelectTeamForPlayer_Implementation(APlayerState* PlayerState)
{
	if (PlayerState->IsOnlyASpectator())
	{
		return INDEX_NONE;
	}

	// default logic assigns players to teams evenly
	return GetLeastPopulatedTeam();
}

int32 UCommonTeamsComponent::GetLeastPopulatedTeam() const
{
	if (TeamDefinitions.IsEmpty())
	{
		return INDEX_NONE;
	}

	TMap<int32, uint32> TeamMemberCounts;
	TeamMemberCounts.Reserve(TeamDefinitions.Num());

	for (const auto& Elem : TeamDefinitions)
	{
		TeamMemberCounts.Add(Elem.Key, 0);
	}

	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	for (const TObjectPtr<APlayerState>& PlayerState : GameState->PlayerArray)
	{
		if (PlayerState->IsInactive())
		{
			continue;
		}

		// find members
		if (const IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(PlayerState))
		{
			const uint8 TeamId = TeamInterface->GetGenericTeamId().GetId();
			if (TeamMemberCounts.Contains(TeamId))
			{
				TeamMemberCounts[TeamId]++;
			}
		}
	}

	// return the lowest count, or the lowest-number team ID if counts match
	int32 BestId = INDEX_NONE;
	uint32 LowestCount = TNumericLimits<uint32>::Max();
	for (const TPair<int32, uint32>& Elem : TeamMemberCounts)
	{
		const int32 TeamId = Elem.Key;
		const uint32 MemberCount = Elem.Value;
		if (MemberCount < LowestCount || BestId == INDEX_NONE || TeamId < BestId)
		{
			BestId = TeamId;
			LowestCount = MemberCount;
		}
	}
	return BestId;
}

IGenericTeamAgentInterface* UCommonTeamsComponent::GetTeamInterfaceForObject(UObject* Object) const
{
	// use object if it has the interface
	if (IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(Object))
	{
		return TeamInterface;
	}

	// try to find player state
	if (AActor* Actor = Cast<AActor>(Object))
	{
		if (const APawn* Pawn = Cast<APawn>(Actor))
		{
			if (IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(Pawn->GetPlayerState()))
			{
				return TeamInterface;
			}
		}
		else if (const AController* Controller = Cast<AController>(Object))
		{
			if (IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(Controller->PlayerState))
			{
				return TeamInterface;
			}
		}
	}

	return nullptr;
}

const IGenericTeamAgentInterface* UCommonTeamsComponent::GetTeamInterfaceForObject(const UObject* Object) const
{
	return GetTeamInterfaceForObject(const_cast<UObject*>(Object));
}

FGenericTeamId UCommonTeamsComponent::GetObjectGenericTeamId(const UObject* Object) const
{
	if (const IGenericTeamAgentInterface* TeamInterface = GetTeamInterfaceForObject(Object))
	{
		return TeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

int32 UCommonTeamsComponent::GetObjectTeam(const UObject* Object) const
{
	return UCommonTeamStatics::GenericTeamIdToInteger(GetObjectGenericTeamId(Object));
}

void UCommonTeamsComponent::SetObjectTeam(UObject* Object, int32 NewTeamId)
{
	if (IGenericTeamAgentInterface* TeamInterface = GetTeamInterfaceForObject(Object))
	{
		return TeamInterface->SetGenericTeamId(UCommonTeamStatics::IntegerToGenericTeamId(NewTeamId));
	}
}

TEnumAsByte<ETeamAttitude::Type> UCommonTeamsComponent::GetAttitude(const UObject* ObjectA, const UObject* ObjectB) const
{
	const FGenericTeamId TeamIdA = GetObjectGenericTeamId(ObjectA);
	const FGenericTeamId TeamIdB = GetObjectGenericTeamId(ObjectB);
	if (TeamIdA == FGenericTeamId::NoTeam || TeamIdB == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}
	return FGenericTeamId::GetAttitude(TeamIdA, TeamIdB);
}

ECommonTeamComparison UCommonTeamsComponent::CompareTeams(const UObject* ObjectA, const UObject* ObjectB) const
{
	const int32 TeamIdA = GetObjectTeam(ObjectA);
	const int32 TeamIdB = GetObjectTeam(ObjectB);

	if (TeamIdA == INDEX_NONE || TeamIdB == INDEX_NONE)
	{
		return ECommonTeamComparison::NoTeam;
	}

	return TeamIdA == TeamIdB ? ECommonTeamComparison::SameTeam : ECommonTeamComparison::DifferentTeams;
}

#if WITH_SERVER_CODE
void UCommonTeamsComponent::ServerAssignTeamsForPlayers()
{
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	for (const TObjectPtr<APlayerState>& PlayerState : GameState->PlayerArray)
	{
		ServerAssignTeamForPlayer(PlayerState);
	}

	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UCommonTeamsComponent::OnPlayerPostLogin);
}

void UCommonTeamsComponent::ServerAssignTeamForPlayer(APlayerState* PlayerState)
{
	if (IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(PlayerState))
	{
		const FGenericTeamId TeamId = UCommonTeamStatics::IntegerToGenericTeamId(SelectTeamForPlayer(PlayerState));
		TeamInterface->SetGenericTeamId(TeamId);
	}
}

void UCommonTeamsComponent::OnPlayerPostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	if (NewPlayer->PlayerState)
	{
		ServerAssignTeamForPlayer(NewPlayer->PlayerState);
	}
}
#endif
