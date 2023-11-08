// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameplayTagInput : ModuleRules
{
	public GameplayTagInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"EnhancedInput",
			"GameplayTags",
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