# Extended Ability System Plugins

Three plugins for extending and working with Unreal's builtin Gameplay Abilities.


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

Some additional, more specific components for working with ability systems. Comes with pawn base classes for
getting up and running with ability systems painless, as well as some other common action game functionality.

### Features

- `UPawnInitStateComponent`, a component that handles async initialization of pawns. e.g. waiting for controller,
    player state, or other replicated data to be available.
- `AAbilityCharacter`, an ACharacter base class that implements `IAbilitySystemInterface`, `IGameplayTagAssetInterface`,
    and uses the `UPawnInitStateComponent` to properly initialize ability systems. Also provides
    `InputAbilityTagPressed/Released` for easy binding of ability input by tags.
- `UCommonHealthComponent` and `UCommonGameplayAbility_Death` for handling the lifecycle of a character that can
    die when an attribute (e.g. HP) reaches zero.


## GameplayTagInput

A small plugin with configuration data assets for setting up ability system input to be driven by gameplay tags.
This abstraction allows separation between specific enhanced input actions and the abilities to activate.

### Features

- `UGameplayTagInputConfig` which maps input actions to gameplay tags.
- TODO: builtin utils / examples for easily binding inputs using these configs.
