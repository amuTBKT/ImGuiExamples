using UnrealBuildTool;

public class ImGuiExamples : ModuleRules
{
	public ImGuiExamples(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"SlateCore",
				"CoreUObject",
			});

		ProjectDescriptor ProjectFile = ProjectDescriptor.FromFile(Target.ProjectFile);
		PluginInfo ImGuiPlugin = Plugins.GetPlugin("ImGuiPlugin");
		if (ImGuiPlugin != null && Plugins.IsPluginEnabledForTarget(ImGuiPlugin, ProjectFile, Target.Platform, Target.Configuration, Target.Type))
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "ImGui", "ImGuiRuntime" });
		}
	}
}
