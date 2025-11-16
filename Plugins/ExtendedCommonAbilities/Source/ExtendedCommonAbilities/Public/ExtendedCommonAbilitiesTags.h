// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


namespace ExtendedCommonAbilities::GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Trait_PersistOnDeath);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Trait_PersistOnDeath);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Type_Damage);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Damage_Incoming);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Damage_Outgoing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death_SelfDestruct);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayMessage_Death);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death_Dying);
}
