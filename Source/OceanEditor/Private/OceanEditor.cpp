#include "OceanEditor.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "OceanProfileCreateAction.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "OceanRuntime/Public/OceanProfile.h"

#define LOCTEXT_NAMESPACE "FOceanEditorModule"

void FOceanEditorModule::StartupModule()
{
	FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
	IAssetTools& AssetTools = AssetToolsModule.Get();

	///Rigister Asset Category
	EAssetTypeCategories::Type OceanCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Infinity Ocean")), LOCTEXT("InfinityOceanCategory", "InfinityOcean"));
	OceanProfileAction = new FOceanProfileCreateAction(OceanCategory);
	AssetTools.RegisterAssetTypeActions(MakeShareable(OceanProfileAction));

	///Rigister Asset Icon
	OceanProfileIcon = MakeShareable(new FSlateStyleSet("OceanProfileStyle"));
	FString ResourceDir = IPluginManager::Get().FindPlugin("InfinityOcean")->GetBaseDir();
	OceanProfileIcon->SetContentRoot(ResourceDir);

	FSlateImageBrush* OceanProfileBrush = new FSlateImageBrush
	(
		OceanProfileIcon->RootToContentDir(TEXT("Resources/Icon_Ocean"), TEXT(".png")), FVector2D(128, 128)
	);

	if (OceanProfileBrush != nullptr)
	{
		OceanProfileIcon->Set("ClassThumbnail.OceanProfile", OceanProfileBrush);
		FSlateStyleRegistry::RegisterSlateStyle(*OceanProfileIcon);
	}
}

void FOceanEditorModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
		IAssetTools& AssetTools = AssetToolsModule.Get();

		///UnRigister Asset Category
		AssetTools.UnregisterAssetTypeActions(OceanProfileAction->AsShared());

		///UnRigister Asset Icon
		FSlateStyleRegistry::UnRegisterSlateStyle(OceanProfileIcon->GetStyleSetName());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOceanEditorModule, OceanEditor)