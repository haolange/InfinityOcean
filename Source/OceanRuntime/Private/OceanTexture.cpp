#include "OceanTexture.h"
#include "RHI.h"
#include "Shader.h"
#include "ClearQuad.h"
#include "OceanShader.h"
#include "RHICommandList.h"
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Public/SceneUtils.h"
//#include "PostProcess/SceneRenderTargets.h"

UOceanTexture::UOceanTexture()
{

}

UOceanTexture::~UOceanTexture()
{
	OmegaBuffer.Release();
	GaussBuffer.Release();
	HZeroBuffer.Release();
	HTBuffer.Release();
	DTBuffer.Release();
	Height_RT.SafeRelease();

	GaussBuffer_SRV.SafeRelease();
	HZeroBuffer_SRV.SafeRelease();
	HZeroBuffer_UAV.SafeRelease();
	OmegaBuffer_SRV.SafeRelease();
	HTBuffer_SRV.SafeRelease();
	HTBuffer_UAV.SafeRelease();
	DTBuffer_SRV.SafeRelease();
	DTBuffer_UAV.SafeRelease();
	Height_UAV.SafeRelease();
	Normal_UAV.SafeRelease();
}

#pragma optimize("", off)
float UOceanTexture::GaussValue()
{
	float U1 = FMath::FRandRange(0, 1);
	float U2 = FMath::FRandRange(0, 1);
	if (U1 < 1e-6f) {
		U1 = 1e-6f;
	}
	return FMath::Sqrt(-2.0f * FMath::Loge(U1)) * FMath::Cos(2.0f * U2 * 3.1415926f);
}

void UOceanTexture::InitGaussArray(int32 ArraySizeXY, TResourceArray<FVector2D>& GaussArrayRef)
{
	GaussArrayRef.Reset();
	GaussArrayRef.Empty();
	GaussArrayRef.SetAllowCPUAccess(true);
	GaussArrayRef.Init(FVector2D(0, 0), (ArraySizeXY + 4) * (ArraySizeXY + 1));

	for (int32 i = 0; i <= ArraySizeXY; i++) {
		for (int32 j = 0; j <= ArraySizeXY; j++) {
			int32 ix = i * (ArraySizeXY + 4) + j;
			GaussArrayRef[ix].X = GaussValue();
			GaussArrayRef[ix].Y = GaussValue();
		}
	}
}

void UOceanTexture::InitOmegaArray(int32 ArraySizeXY, float Period, TResourceArray<float>& OmegaArrayRef)
{
	OmegaArrayRef.Reset();
	OmegaArrayRef.Empty();
	OmegaArrayRef.SetAllowCPUAccess(true);
	OmegaArrayRef.Init(0, (ArraySizeXY + 4) * (ArraySizeXY + 1));

	for (int32 i = 0; i <= ArraySizeXY; i++) {
		int32 ny = (-ArraySizeXY / 2 + i);

		FVector2D K;
		K.Y = (float)(ny) * (6.2831852f / Period);

		for (int32 j = 0; j <= ArraySizeXY; j++) {
			int32 nx = (-ArraySizeXY / 2 + j);
			K.X = (float)nx * (2 * 3.14 / Period);
			OmegaArrayRef[i * (ArraySizeXY + 4) + j] = FMath::Sqrt(FMath::Sqrt(K.X * K.X + K.Y * K.Y) * 9.810f);
		}
	}
}

void UOceanTexture::Init(ERHIFeatureLevel::Type FeatureLevel, int32 Resolution, uint8 HeightFormat, uint8 NormalFormat, const FOceanParameterStruct& OceanParameters)
{
	check(IsInRenderingThread());

	TResourceArray<FVector2D> GaussArray;
	InitGaussArray(Resolution, GaussArray);
	for (int32 i = 0; i < Resolution; ++i) 
	{
		FMemory::Memmove(GaussArray.GetData(), GaussArray.GetData() + i * (Resolution + 4), Resolution * sizeof(FVector2D));
	}
	GaussBuffer.Initialize(sizeof(FVector2D), GaussArray.Num(), GaussArray);
	GaussBuffer_SRV = RHICreateShaderResourceView(GaussBuffer.Buffer);

	HZeroBuffer.Initialize(sizeof(FVector2D), (Resolution + 1) * (Resolution + 1));
	HZeroBuffer_SRV = RHICreateShaderResourceView(HZeroBuffer.Buffer);
	HZeroBuffer_UAV = RHICreateUnorderedAccessView(HZeroBuffer.Buffer, false, false);

	TResourceArray<float> OmegaArray;
	InitOmegaArray(Resolution, OceanParameters.Period, OmegaArray);
	for (int32 i = 0; i < (Resolution / 2 + 1); ++i) 
	{
		FMemory::Memmove(OmegaArray.GetData() + i * (Resolution / 2 + 1), OmegaArray.GetData() + i * (Resolution + 4), (Resolution / 2 + 1) * sizeof(float));
	}
	OmegaBuffer.Initialize(sizeof(float), OmegaArray.Num(), OmegaArray);
	OmegaBuffer_SRV = RHICreateShaderResourceView(OmegaBuffer.Buffer);

	HTBuffer.Initialize(sizeof(FVector2D), (Resolution / 2 + 1) * Resolution);
	HTBuffer_SRV = RHICreateShaderResourceView(HTBuffer.Buffer);
	HTBuffer_UAV = RHICreateUnorderedAccessView(HTBuffer.Buffer, false, false);

	DTBuffer.Initialize(sizeof(FVector4), (Resolution / 2 + 1) * Resolution);
	DTBuffer_SRV = RHICreateShaderResourceView(DTBuffer.Buffer);
	DTBuffer_UAV = RHICreateUnorderedAccessView(DTBuffer.Buffer, false, false);

	FRHIResourceCreateInfo HeightRT_CreateInfo;
	Height_RT = RHICreateTexture2D(Resolution, Resolution, HeightFormat, 1, 1, TexCreate_UAV | TexCreate_ShaderResource, HeightRT_CreateInfo);
	Height_UAV = RHICreateUnorderedAccessView(Height_RT);

	FRHIResourceCreateInfo NormalRT_CreateInfo;
	Normal_RT = RHICreateTexture2D(Resolution, Resolution, NormalFormat, 1, 1, TexCreate_UAV | TexCreate_ShaderResource, NormalRT_CreateInfo);
	Normal_UAV = RHICreateUnorderedAccessView(Normal_RT);
}

void UOceanTexture::Draw(ERHIFeatureLevel::Type FeatureLevel, int32 Resolution, float SimulationTime, const FOceanParameterStruct& OceanParameters, FRHITexture* DscHeightTexture, FRHITexture* DscNormalTexture, FRHICommandListImmediate& CmdList)
{
	check(IsInRenderingThread());

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_ComputeShader); 
	SCOPED_DRAW_EVENT(CmdList, ShaderPlugin_Compute);

	//Paper UnifromBuffer
	float FFT_Norm = FMath::Pow((float)Resolution, -0.25f);
	float FFT_PhilNorm = Euler / FMath::Max(0.00001f, OceanParameters.Period);
	float FFT_Gravity = FMath::Pow(Gravity / FMath::Pow(OceanParameters.WindSpeed, 2), 2);

	FOceanUniformBuffer OceanUniformBuffer;
	{
		OceanUniformBuffer.Resolution = Resolution;
		OceanUniformBuffer.Resolution_PlusOne = Resolution + 1;
		OceanUniformBuffer.Resolution_PlusOne_Squared_MinusOne = ((Resolution + 1) * (Resolution + 1) - 1);
		OceanUniformBuffer.ResolutionHalf = Resolution / 2;
		OceanUniformBuffer.ResolutionHalf_PlusOne = Resolution / 2 + 1;
		for (int i = 0; (1 << i) <= Resolution; ++i) { OceanUniformBuffer.ResolutionLog2_Add32 = 32 - i; }
		OceanUniformBuffer.Time = SimulationTime * OceanParameters.TimeScale;
		OceanUniformBuffer.FrequencyScale = TwoPi / FMath::Max(0.00001f, OceanParameters.Period);
		OceanUniformBuffer.LinearScale = FFT_Norm * FFT_PhilNorm * SqrtHalf * OceanParameters.Aplitude;
		OceanUniformBuffer.WindScale = -FMath::Sqrt(1 - OceanParameters.WindDependency);
		OceanUniformBuffer.RootScale = -0.5f * FFT_Gravity;
		OceanUniformBuffer.PowerScale = FMath::Max(0.0001f, -0.5f / FFT_Gravity * FMath::Pow(OceanParameters.Fraction, 2));
		OceanUniformBuffer.ChoppyScale = OceanParameters.Choppyness;
		OceanUniformBuffer.WindDir = OceanParameters.WindDir;
	}
	TUniformBufferRef<FOceanUniformBuffer> OceanUniformBufferRef = TUniformBufferRef<FOceanUniformBuffer>::CreateUniformBufferImmediate(OceanUniformBuffer, UniformBuffer_SingleFrame);

	//Dispatch ComputeHZero 
	TShaderMapRef<FOceanShader_HZero> OceanShader_HZero(GetGlobalShaderMap(FeatureLevel));
	{
		FOceanShader_HZero::FParameters HZeroParameter;
		{
			HZeroParameter.UniformBuffer = OceanUniformBufferRef;
			HZeroParameter.Input_SRV_GaussBuffer = GaussBuffer_SRV;
			HZeroParameter.Output_UAV_HZeroBuffer = HZeroBuffer_UAV;
            //HZeroParameter.Output_UAV_DisplacementTexture = Height_UAV;
		}
		FComputeShaderUtils::Dispatch(CmdList, OceanShader_HZero, HZeroParameter, FIntVector(1, Resolution, 1));
		//CmdList.TransitionResource(EResourceTransitionAccess::EReadable, EResourceTransitionPipeline::EComputeToCompute, HZeroBuffer_UAV);
	}

	//Dispatch ComputeSpecturm 
	TShaderMapRef<FOceanShader_Specturm> OceanShader_Specturm(GetGlobalShaderMap(FeatureLevel));
	{
		FOceanShader_Specturm::FParameters HTDTParameter;
		{
			HTDTParameter.UniformBuffer = OceanUniformBufferRef;
			HTDTParameter.Input_SRV_OmegaBuffer = OmegaBuffer_SRV;
			HTDTParameter.Input_SRV_HZeroBuffer = HZeroBuffer_SRV;
			HTDTParameter.Output_UAV_HTBuffer = HTBuffer_UAV;
			HTDTParameter.Output_UAV_DTBuffer = DTBuffer_UAV;
		}
		FComputeShaderUtils::Dispatch(CmdList, OceanShader_Specturm, HTDTParameter, FIntVector(1, Resolution / 2 + 1, 1));
		//CmdList.TransitionResource(EResourceTransitionAccess::EReadable, EResourceTransitionPipeline::EComputeToCompute, HTBuffer_UAV);
		//CmdList.TransitionResource(EResourceTransitionAccess::EReadable, EResourceTransitionPipeline::EComputeToCompute, DTBuffer_UAV);
	}

	//Dispatch ComputeDisplacement 
	TShaderMapRef<FOceanShader_Displacement> OceanShader_Displacement(GetGlobalShaderMap(FeatureLevel));
	{
		FOceanShader_Displacement::FParameters DisplacementParameter;
		{
			DisplacementParameter.UniformBuffer = OceanUniformBufferRef;
			DisplacementParameter.Input_SRV_HTBuffer = HTBuffer_SRV;
			DisplacementParameter.Input_SRV_DTBuffer = DTBuffer_SRV;
			DisplacementParameter.Output_UAV_DisplacementTexture = Height_UAV;
		}
		FComputeShaderUtils::Dispatch(CmdList, OceanShader_Displacement, DisplacementParameter, FIntVector(1, Resolution, 1));
		//CmdList.TransitionResource(EResourceTransitionAccess::EReadable, EResourceTransitionPipeline::EComputeToGfx, Displacement_UAV);
	}

	//Dispatch ComputeNormalFloading
	TShaderMapRef<FOceanShader_NormalFoading> OceanShader_NormalFloding(GetGlobalShaderMap(FeatureLevel));
	{
		FOceanShader_NormalFoading::FParameters NormalFlodingParameter;
		{
			NormalFlodingParameter.SizeScale = FVector4((float)Resolution, (float)Resolution, 1 / (float)Resolution, 1 / (float)Resolution);
			NormalFlodingParameter.VectorTexture = Height_RT;
			NormalFlodingParameter.VectorTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();
			NormalFlodingParameter.Output_UAV_NormalFomeTexture = Normal_UAV;
		}
		FComputeShaderUtils::Dispatch(CmdList, OceanShader_NormalFloding, NormalFlodingParameter, FIntVector(Resolution / 16, Resolution / 16, 1));
		//CmdList.TransitionResource(EResourceTransitionAccess::EReadable, EResourceTransitionPipeline::EComputeToGfx, Displacement_UAV);
	}

	CmdList.CopyTexture(Height_RT, DscHeightTexture, FRHICopyTextureInfo());
	CmdList.CopyTexture(Normal_RT, DscNormalTexture, FRHICopyTextureInfo());
	//CmdList.CopyToResolveTarget(Height_RT, DestHeightTexture, FResolveParams());
	//CmdList.CopyToResolveTarget(Normal_RT, DestNormalTexture, FResolveParams());
	//ClearUAV(CmdList, Displacement_UAV, Resolution, Resolution, FLinearColor(1, 0, 0, 0));
	//FSceneRenderTargets& SceneContext = FSceneRenderTargets::Get(CmdList);
}
#pragma optimize("", on)
