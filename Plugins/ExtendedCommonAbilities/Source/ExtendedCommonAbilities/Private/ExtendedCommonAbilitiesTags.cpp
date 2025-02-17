// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedCommonAbilitiesTags.h"


namespace ExtendedCommonAbilitiesTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Trait_PersistOnDeath, "Ability.Trait.PersistOnDeath", "This ability should not be canceled upon death");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_Trait_PersistOnDeath, "Effect.Trait.PersistOnDeath", "This effect should not be removed upon death");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_Type_Damage, "Effect.Type.Damage", "Gameplay effect that deals damage");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Event_Damage_Outgoing, "Event.Damage.Outgoing", "Triggered when damage is dealt by this ability system");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Event_Death, "Event.Death", "Triggered when a character dies from health reaching 0 or other causes");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Event_Death_SelfDestruct, "Event.Death.SelfDestruct", "Triggered when character dies from self destruct");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GameplayMessage_Death, "GameplayMessage.Death", "Broadcast when a character dies");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_Spawned, "InitState.Spawned",
	                               "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_DataAvailable, "InitState.DataAvailable",
	                               "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_DataInitialized, "InitState.DataInitialized",
	                               "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_GameplayReady, "InitState.GameplayReady",
	                               "4: The actor/component is fully ready for active gameplay");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_Death_Dead, "State.Death.Dead", "Death and any FX or animation have finished");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_Death_Dying, "State.Death.Dying", "Death has started, final FX or animation is being played");
}
