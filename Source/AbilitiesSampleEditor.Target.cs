// Copyright Bohdon Sayre, All Rights Reserved.

using UnrealBuildTool;

public class AbilitiesSampleEditorTarget : TargetRules
{
	public AbilitiesSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[]
		{
			"AbilitiesSample",
		});
	}
}