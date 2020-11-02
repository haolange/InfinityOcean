// Copyright Epic Games, Inc. All Rights Reserved.

#include "OceanRuntime.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FOceanRuntimeModule"

void FOceanRuntimeModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("InfinityEnvironment"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/InfinityEnvironment"), PluginShaderDir);
}

void FOceanRuntimeModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOceanRuntimeModule, OceanRuntime)