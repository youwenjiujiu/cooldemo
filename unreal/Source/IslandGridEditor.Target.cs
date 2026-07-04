using UnrealBuildTool;

public class IslandGridEditorTarget : TargetRules
{
	public IslandGridEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("IslandGrid");
	}
}
