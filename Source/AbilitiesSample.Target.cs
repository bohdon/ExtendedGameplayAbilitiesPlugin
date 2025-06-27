// Copyright Bohdon Sayre, All Rights Reserved.

using UnrealBuildTool;

public class AbilitiesSampleTarget : TargetRules
{
	public AbilitiesSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[]
		{
			"AbilitiesSample",
		});
	}
}