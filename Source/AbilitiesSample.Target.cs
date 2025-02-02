// Copyright Bohdon Sayre, All Rights Reserved.

using UnrealBuildTool;

public class AbilitiesSampleTarget : TargetRules
{
	public AbilitiesSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[]
		{
			"AbilitiesSample",
		});
	}
}