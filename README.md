# Extended Gameplay Abilities Plugins

Two plugins for extending and working with Unreal's builtin Gameplay Abilities.


## ExtendedGameplayAbilities

Extends the ability system with some important core features for making out of the box ability usage better.

### Features

- Effect Sets - Define groups of gameplay effects together for simpler application.
- Gameplay Tag Input - Activate abilities using gameplay tag input, designed for use with enhanced input.
- Ability Sets - Improved ability sets that support abilities, effects, and attribute sets.
- Gameplay Tag Anim Notify State - Apply gameplay tags via anim notify states for timed states.
- Startup Ability Sets - Define abilities, effects, and attributes that should be granted by default per ability system.
- Behavior Tree AI tasks for activating and cancelling abilities.
- `UAbilityGamePhaseComponent` and `UGamePhaseAbility` for handling various game phases like pre and post game.
- MVVM view models for activate gameplay effects.


## ExtendedCommonAbilities

Some additional, more specific abilities and classes for working with ability systems. Comes with pawn base classes for
getting up and running with ability systems painless, as well as some other common action game functionality.

### Features

- `UPawnInitStateComponent`, a component that handles async initialization of pawns. e.g. waiting for controller,
    player state, or other replicated data to be available.
- `AAbilityCharacter`, an ACharacter base class that implements `IAbilitySystemInterface`, `IGameplayTagAssetInterface`,
    and uses the `UPawnInitStateComponent` to properly initialize ability systems. Also provides
    `InputAbilityTagPressed/Released` for easy binding of ability input by tags.
- `UCommonHealthComponent` and `UCommonGameplayAbility_Death` for handling the lifecycle of a character that can
    die when an attribute (e.g. HP) reaches zero.

### Plugin Dependencies

The `AAbilityCharacter` inherits from `AModularCharacter`, which comes from the `ModularGameplayActors` plugin, and the
death ability has built-in support for broadcasting gameplay message via the `GameplayMessageRouter`.

- [GameplayMessageRouter](https://github.com/EpicGames/UnrealEngine/tree/ue5-main/Samples/Games/Lyra/Plugins/GameplayMessageRouter) from Lyra \*
- [ModularGameplayActors](https://github.com/EpicGames/UnrealEngine/tree/ue5-main/Samples/Games/Lyra/Plugins/ModularGameplayActors) from Lyra \*

_\* These links currently point the ue5-main branch. Switch to a release branch, or use the markeplace to download Lyra for your specific engine release version._
