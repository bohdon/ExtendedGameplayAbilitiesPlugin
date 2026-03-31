// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


#define UE_API EXTENDEDCOMMONABILITIES_API

namespace ExtendedCommonAbilities::GameplayTags
{
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Trait_PersistOnDeath);

	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Trait_PersistOnDeath);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Type_Damage);

	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Damage_Incoming);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Damage_Outgoing);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death_SelfDestruct);

	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayMessage_Death);

	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death_Dead);
	UE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death_Dying);
}

#undef UE_API
