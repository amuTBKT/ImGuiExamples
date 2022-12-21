using UnrealBuildTool;
using System.Collections.Generic;

public class ImGuiExamplesTarget : TargetRules
{
	public ImGuiExamplesTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("ImGuiExamples");
	}
}
