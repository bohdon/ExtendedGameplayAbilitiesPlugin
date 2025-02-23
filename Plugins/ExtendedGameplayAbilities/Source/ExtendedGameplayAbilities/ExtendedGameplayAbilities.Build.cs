// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExtendedGameplayAbilities : ModuleRules
{
	public ExtendedGameplayAbilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"AIModule",
			"Core",
			"DataRegistry",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"ModelViewViewModel",
			"TargetingSystem",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Settings",
			"Slate",
			"SlateCore",
		});
	}
}