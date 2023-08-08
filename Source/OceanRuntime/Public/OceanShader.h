#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h" 
#include "UObject/ObjectMacros.h"
#include "ShaderParameterStruct.h"
#include "ShaderParameterMacros.h"

// Unifrom Buffer
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT_WITH_CONSTRUCTOR(FOceanUniformData, )
	SHADER_PARAMETER(int32, Resolution)
	SHADER_PARAMETER(int32, Resolution_PlusOne)
	SHADER_PARAMETER(int32, Resolution_PlusOne_Squared_MinusOne)
	SHADER_PARAMETER(int32, ResolutionHalf)
	SHADER_PARAMETER(int32, ResolutionHalf_PlusOne)
	SHADER_PARAMETER(int32, ResolutionLog2_Add32)

	SHADER_PARAMETER(float, Time)
	SHADER_PARAMETER(float, FrequencyScale)
	SHADER_PARAMETER(float, LinearScale)
	SHADER_PARAMETER(float, WindScale)
	SHADER_PARAMETER(float, RootScale)
	SHADER_PARAMETER(float, PowerScale)
	SHADER_PARAMETER(float, ChoppyScale)

	SHADER_PARAMETER(FVector2D, WindDir)
END_GLOBAL_SHADER_PARAMETER_STRUCT()
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FOceanUniformData, "OceanParameter");

// Compute HZero
class FOceanShader_HZero : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FOceanShader_HZero, Global);
	SHADER_USE_PARAMETER_STRUCT(FOceanShader_HZero, FGlobalShader);

public:

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FOceanUniformData, UniformBuffer)
		SHADER_PARAMETER_SRV(Buffer<float2>, Input_SRV_GaussBuffer)
		SHADER_PARAMETER_UAV(RWBuffer<float2>, Output_UAV_HZeroBuffer)
        //SHADER_PARAMETER_UAV(RWTexture2D<float4>, Output_UAV_DisplacementTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};
IMPLEMENT_SHADER_TYPE(, FOceanShader_HZero, TEXT("/Plugin/InfinityOcean/Private/Ocean_ComputeHZero.usf"), TEXT("ComputeHZero"), SF_Compute)

// Compute Specturm
class FOceanShader_Specturm : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FOceanShader_Specturm, Global)
	SHADER_USE_PARAMETER_STRUCT(FOceanShader_Specturm, FGlobalShader);

public:

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FOceanUniformData, UniformBuffer)
		SHADER_PARAMETER_SRV(Buffer<float>, Input_SRV_OmegaBuffer)
		SHADER_PARAMETER_SRV(Buffer<float2>, Input_SRV_HZeroBuffer)
		SHADER_PARAMETER_UAV(RWBuffer<float2>, Output_UAV_HTBuffer)
		SHADER_PARAMETER_UAV(RWBuffer<float4>, Output_UAV_DTBuffer)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};
IMPLEMENT_SHADER_TYPE(, FOceanShader_Specturm, TEXT("/Plugin/InfinityOcean/Private/Ocean_ComputeSpecturm.usf"), TEXT("ComputeSpecturm"), SF_Compute)

// Compute Displacement
class FOceanShader_Displacement : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FOceanShader_Displacement, Global)
	SHADER_USE_PARAMETER_STRUCT(FOceanShader_Displacement, FGlobalShader);

public:
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FOceanUniformData, UniformBuffer)
		SHADER_PARAMETER_SRV(Buffer<float2>, Input_SRV_HTBuffer)
		SHADER_PARAMETER_SRV(Buffer<float4>, Input_SRV_DTBuffer)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, Output_UAV_DisplacementTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};
IMPLEMENT_SHADER_TYPE(, FOceanShader_Displacement, TEXT("/Plugin/InfinityOcean/Private/Ocean_ComputeDisplacement.usf"), TEXT("ComputeDisplacement"), SF_Compute)

// Compute NormalFoading
class FOceanShader_NormalFoading : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FOceanShader_NormalFoading, Global)
	SHADER_USE_PARAMETER_STRUCT(FOceanShader_NormalFoading, FGlobalShader);

public:

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FVector4, SizeScale)
		SHADER_PARAMETER_TEXTURE(Texture2D, VectorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, VectorTextureSampler)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, Output_UAV_NormalFomeTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};
IMPLEMENT_SHADER_TYPE(, FOceanShader_NormalFoading, TEXT("/Plugin/InfinityOcean/Private/Ocean_ComputeNormalFoading.usf"), TEXT("ComputeNormalFoading"), SF_Compute)
