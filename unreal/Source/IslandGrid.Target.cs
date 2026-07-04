using UnrealBuildTool;

public class IslandGridTarget : TargetRules
{
	public IslandGridTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("IslandGrid");
	}
}
