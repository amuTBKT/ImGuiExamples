using UnrealBuildTool;
using System.Collections.Generic;

public class ImGuiExamplesTarget : TargetRules
{
	public ImGuiExamplesTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		
		ExtraModuleNames.Add("ImGuiExamples");
	}
}
