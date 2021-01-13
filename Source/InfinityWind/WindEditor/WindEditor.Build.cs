using UnrealBuildTool;

public class WindEditor : ModuleRules
{
	public WindEditor(ReadOnlyTargetRules Target) : base(Target)
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
    }
}
