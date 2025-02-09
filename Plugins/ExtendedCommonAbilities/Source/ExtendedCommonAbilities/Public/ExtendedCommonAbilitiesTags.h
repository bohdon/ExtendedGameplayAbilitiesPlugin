// Copyright Bohdon Sayre, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


namespace ExtendedCommonAbilitiesTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Trait_PersistOnDeath);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Death_SelfDestruct);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayMessage_Death);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_Spawned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_DataAvailable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_DataInitialized);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_GameplayReady);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dying);
}
