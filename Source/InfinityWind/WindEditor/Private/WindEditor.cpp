#include "WindEditor.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FWindEditorModule"

void FWindEditorModule::StartupModule()
{

}

void FWindEditorModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWindEditorModule, WindEditor)