#include "WindRuntime.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FWindRuntimeModule"

void FWindRuntimeModule::StartupModule()
{
	//FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("InfinityEnvironment"))->GetBaseDir(), TEXT("Shaders"));
	//AddShaderSourceDirectoryMapping(TEXT("/Plugin/InfinityEnvironment"), PluginShaderDir);
}

void FWindRuntimeModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWindRuntimeModule, WindRuntime)