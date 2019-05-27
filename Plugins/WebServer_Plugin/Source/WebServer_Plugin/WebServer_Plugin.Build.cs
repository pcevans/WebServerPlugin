using UnrealBuildTool;

public class WebServer_Plugin : ModuleRules
{
	public WebServer_Plugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Sockets", "Networking"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
