#include "OceanShader.h"

#include "Shader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ShaderParameterUtils.h"

////////////////////////////Unifrom Buffer
FOceanUniformBuffer::FOceanUniformBuffer()
{
	FMemory::Memzero(*this);

	Resolution = 0;
	Resolution_PlusOne = 0;
	ResolutionHalf = 0;
	ResolutionHalf_PlusOne = 0;
	Resolution_PlusOne_Squared_MinusOne = 0;
	ResolutionLog2_Add32 = 0;

	Time = 0.0;
	FrequencyScale = 0.0;
	LinearScale = 0.0f;
	WindScale = 0.0f;
	RootScale = 0.0f;
	PowerScale = 0.0f;
	ChoppyScale = 0.0f;

	WindDir = FVector2D(0.8f, -0.6f);
}


////////////////////////////Compute HZero
bool FOceanShader_HZero::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
}

void FOceanShader_HZero::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
}


////////////////////////////Compute Specturm
bool FOceanShader_Specturm::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
}

void FOceanShader_Specturm::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
}


////////////////////////////Compute Displacement
bool FOceanShader_Displacement::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
}

void FOceanShader_Displacement::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
}


////////////////////////////Compute NormalFoading
bool FOceanShader_NormalFoading::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
}

void FOceanShader_NormalFoading::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
}