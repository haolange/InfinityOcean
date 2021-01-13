#include "OceanEditor.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "AssetActions_CreateOceanProfile.h"
#include "OceanRuntime\Public\OceanProfile.h"

#define LOCTEXT_NAMESPACE "FOceanEditorModule"

void FOceanEditorModule::StartupModule()
{
	FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
	IAssetTools& AssetTools = AssetToolsModule.Get();

	///Rigister Asset Category
	InfinityOceanCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Infinity Ocean")), LOCTEXT("InfinityOceanCategory", "InfinityOcean"));
	OceanAssetAction = new FAssetActions_CreateOceanProfile(InfinityOceanCategory);
	AssetTools.RegisterAssetTypeActions(MakeShareable(OceanAssetAction));

	///Rigister Asset Icon
	OceanProfileStyleIcon = MakeShareable(new FSlateStyleSet("OceanProfileStyle"));
	FString ResourceDir = IPluginManager::Get().FindPlugin("InfinityEnvironment")->GetBaseDir();
	OceanProfileStyleIcon->SetContentRoot(ResourceDir);

	FSlateImageBrush* OceanProfileBrush = new FSlateImageBrush
	(
		OceanProfileStyleIcon->RootToContentDir(TEXT("Resources/Icon_Ocean"), TEXT(".png")), FVector2D(128, 128)
	);

	if (OceanProfileBrush != nullptr)
	{
		OceanProfileStyleIcon->Set("ClassThumbnail.OceanProfile", OceanProfileBrush);
		FSlateStyleRegistry::RegisterSlateStyle(*OceanProfileStyleIcon);
	}
}

void FOceanEditorModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
		IAssetTools& AssetTools = AssetToolsModule.Get();

		///UnRigister Asset Category
		AssetTools.UnregisterAssetTypeActions(OceanAssetAction->AsShared());

		///UnRigister Asset Icon
		FSlateStyleRegistry::UnRegisterSlateStyle(OceanProfileStyleIcon->GetStyleSetName());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOceanEditorModule, OceanEditor)