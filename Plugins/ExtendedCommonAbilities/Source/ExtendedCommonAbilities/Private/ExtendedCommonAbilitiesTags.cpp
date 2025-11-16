// Copyright Bohdon Sayre, All Rights Reserved.


#include "ExtendedCommonAbilitiesTags.h"


namespace ExtendedCommonAbilities::GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Trait_PersistOnDeath, "Ability.Trait.PersistOnDeath", "This ability should not be canceled upon death");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Trait_PersistOnDeath, "Effect.Trait.PersistOnDeath", "This effect should not be removed upon death");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Type_Damage, "Effect.Type.Damage", "Gameplay effect that deals damage");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Damage_Incoming, "Event.Damage.Incoming", "Triggered when damage is received by this ability system");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Damage_Outgoing, "Event.Damage.Outgoing", "Triggered when damage is dealt by this ability system");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Death, "Event.Death", "Triggered when a character dies from health reaching 0 or other causes");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Death_SelfDestruct, "Event.Death.SelfDestruct", "Triggered when character dies from self destruct");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayMessage_Death, "GameplayMessage.Death", "Broadcast when a character dies");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Death_Dead, "State.Death.Dead", "Death and any FX or animation have finished");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Death_Dying, "State.Death.Dying", "Death has started, final FX or animation is being played");
}
