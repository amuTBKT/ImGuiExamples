using UnrealBuildTool;
using System.Collections.Generic;

public class ImGuiExamplesEditorTarget : TargetRules
{
	public ImGuiExamplesEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		
		ExtraModuleNames.Add("ImGuiExamples");
	}
}
