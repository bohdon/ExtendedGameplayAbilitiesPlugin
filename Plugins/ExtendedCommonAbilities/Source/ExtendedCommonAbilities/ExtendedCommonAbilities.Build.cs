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
			"AsyncMessageSystem",
			"Core",
			"EnhancedInput",
			"ExtendedGameplayAbilities",
			"GameplayAbilities",
			"GameplayTags",
			"ModelViewViewModel",
			"ModularGameplay",
			"ModularGameplayActors",
			"NetCore",
			"TargetingSystem",
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