using System.IO;
using UnrealBuildTool;

public class ImGuiEditorWidgets : ModuleRules
{
	public ImGuiEditorWidgets(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"SlateCore",
			}
		);

		// for using ImGui
        PrivateDependencyModuleNames.AddRange(new string[] { "ImGui", "ImGuiRuntime" });

        // widget specific modules
        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"AssetRegistry"
			}
		);
	}
}