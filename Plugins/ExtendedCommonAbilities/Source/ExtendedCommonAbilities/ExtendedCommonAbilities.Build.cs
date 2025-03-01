// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class ExtendedCommonAbilities : ModuleRules
{
	public ExtendedCommonAbilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"AIModule",
			"Core",
			"EnhancedInput",
			"ExtendedGameplayAbilities",
			"GameplayAbilities",
			"GameplayMessageRuntime",
			"GameplayTags",
			"ModularGameplay",
			"ModularGameplayActors",
			"NetCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
		});
	}
}