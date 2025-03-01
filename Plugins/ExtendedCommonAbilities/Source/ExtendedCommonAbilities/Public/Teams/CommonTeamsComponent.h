// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTeamTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Components/GameStateComponent.h"
#include "CommonTeamsComponent.generated.h"

class UCommonTeamDef;


/**
 * Defines and manages the assignment of teams to pawns or objects in the game.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class EXTENDEDCOMMONABILITIES_API UCommonTeamsComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UCommonTeamsComponent(const FObjectInitializer& ObjectInitializer);

	/** Map of teams to create. Team definition is optional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teams")
	TMap<uint8, TObjectPtr<const UCommonTeamDef>> TeamDefinitions;

	UFUNCTION(BlueprintPure, Category = "Teams")
	const UCommonTeamDef* GetTeamDefinition(int32 TeamId) const;

	/** Return the team with the least number of players. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams")
	int32 GetLeastPopulatedTeam() const;

	/** Find and return the team agent interface to use for an object. */
	virtual IGenericTeamAgentInterface* GetTeamInterfaceForObject(UObject* Object) const;
	virtual const IGenericTeamAgentInterface* GetTeamInterfaceForObject(const UObject* Object) const;

	/** Return the generic team id for an actor or object. */
	virtual FGenericTeamId GetObjectGenericTeamId(const UObject* Object) const;

	/** Return the team id for an actor or object. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams", meta = (DefaultToSelf = "Object"))
	virtual int32 GetObjectTeam(const UObject* Object) const;

	/** Set a new team id for an actor or object. */
	UFUNCTION(BlueprintCallable, Category = "Teams", meta = (DefaultToSelf = "Object"))
	virtual void SetObjectTeam(UObject* Object, int32 NewTeamId);

	/** Return the attitude of ObjectA towards ObjectB. See also 'CompareTeams'. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams", meta = (DefaultToSelf = "ObjectA"))
	TEnumAsByte<ETeamAttitude::Type> GetAttitude(const UObject* ObjectA, const UObject* ObjectB) const;

	/**
	 * Compare the teams of two objects.
	 * This could be useful in addition to 'GetAttitude', since different teams may still be friendly with each other.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams", meta = (DefaultToSelf = "ObjectA"))
	ECommonTeamComparison CompareTeams(const UObject* ObjectA, const UObject* ObjectB) const;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	/** Called during BeginPlay to start setting up teams. */
	virtual void StartTeamSetup();

	/** Return which team a player should be assigned to. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Teams")
	int32 SelectTeamForPlayer(APlayerState* PlayerState);

#if WITH_SERVER_CODE
	/** Assign all player states to teams. */
	virtual void ServerAssignTeamsForPlayers();

	/** Select and assign a team to a player state. */
	virtual void ServerAssignTeamForPlayer(APlayerState* PlayerState);

	/** Called when a player logs in. */
	virtual void OnPlayerPostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
#endif
};
