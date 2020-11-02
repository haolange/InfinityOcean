using UnrealBuildTool;

public class OceanEditor : ModuleRules
{
	public OceanEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "AssetTools",
                "OceanRuntime",
                "Slate",
                "SlateCore",
                "PropertyEditor",
                "EditorStyle",
                "Projects"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "OceanEditor/Private"
         });
    }
}
