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

		// disabled on Shipping and Server configs
		bool bIsConfigurationSupported = (Target.Configuration != UnrealTargetConfiguration.Shipping);
		bool bIsTargetTypeSupported = ((Target.Type != TargetType.Server) && (Target.Type != TargetType.Program));
		if (bIsConfigurationSupported && bIsTargetTypeSupported)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "ImGui", "ImGuiRuntime" });
		}
	}
}
