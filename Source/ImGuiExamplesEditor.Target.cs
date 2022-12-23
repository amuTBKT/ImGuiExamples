using UnrealBuildTool;
using System.Collections.Generic;

public class ImGuiExamplesEditorTarget : TargetRules
{
	public ImGuiExamplesEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("ImGuiExamples");
		ExtraModuleNames.Add("ImGuiEditorWidgets");
	}
}
