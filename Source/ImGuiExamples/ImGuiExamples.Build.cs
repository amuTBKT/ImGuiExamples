using UnrealBuildTool;

public class ImGuiExamples : ModuleRules
{
	public ImGuiExamples(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ImGui", "ImGuiRuntime" });
    }
}
