// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class ExtendedCommonAbilities : ModuleRules
{
	public ExtendedCommonAbilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"ExtendedGameplayAbilities",
			"GameplayAbilities",
			"GameplayMessageRuntime",
			"GameplayTags",
			"ModularGameplay",
			"ModularGameplayActors",
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